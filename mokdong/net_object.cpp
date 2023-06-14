#include "net_object.h"
#include "util.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>

using namespace mokdong::network;


// net_object

net_object::net_object(boost::asio::io_service& io_service)
: io_service_(io_service), 
socket_(io_service)
{
}

void net_object::read_async()
{
	socket_.async_receive(
		boost::asio::buffer(buffer_, max_length),
		boost::bind(&net_object::handle_read_raw,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void net_object::handle_read_raw(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error != boost::system::errc::success)
	{
		auto address = socket_.remote_endpoint().address();
		mokdong::thread_log("disconnected[read]: %s %d",
			address.to_string().c_str(),
			socket_.remote_endpoint().port()
		);

		socket_.close();
		return;
	}

	handle_read(error, bytes_transferred);
}

void net_object::write_async(const char* buffer, size_t size)
{
	socket_.async_send(
		boost::asio::buffer(buffer, size),
		boost::bind(&net_object::handle_write_raw,
			this,
			boost::asio::placeholders::error
		)
	);
}

void net_object::handle_write_raw(const boost::system::error_code& error)
{
	if (error != boost::system::errc::success)
	{
		auto address = socket_.remote_endpoint().address();
		mokdong::thread_log("disconnected[write]: %s %d",
			address.to_string().c_str(),
			socket_.remote_endpoint().port()
		);

		socket_.close();
		return;
	}

	handle_write(error);
}

void net_object::close()
{
	socket_.close();
}



// service_net_object

service_net_object::service_net_object(boost::asio::io_service& io_service)
	: io_service_(io_service),
	socket_(io_service)
{
}

void service_net_object::read_length_async()
{
	socket_.async_receive(
		boost::asio::buffer(read_buffer_.raw_length(), tcp_buffer::length_length),
		boost::bind(&service_net_object::handle_read_length,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

bool service_net_object::read_data_async()
{
	uint16_t remained = read_buffer_.recieve_remained();

	if (remained <= 0)
	{
		return false;
	}

	socket_.async_receive(
		boost::asio::buffer(read_buffer_.raw_data(), remained),
		boost::bind(&service_net_object::handle_read_data,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);

	return true;
}

void service_net_object::write_async(tcp_buffer& buffer)
{
	socket_.async_send(
		boost::asio::buffer(buffer.raw_send(), buffer.send_remained()), 
		boost::bind(&service_net_object::handle_write,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void service_net_object::close()
{
	if (!socket_.is_open()) return;

	socket_.close();

	handle_on_close();
}

void service_net_object::handle_read_length(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if (ec.value() != boost::system::errc::success)
	{
		error_log("service_net_object::handle_read_length", ec);
		handle_on_disconnected(disconnected_from::read_length);
		close();

		return;
	}

	if (bytes_transferred != tcp_buffer::length_length)
	{
		error_log("service_net_object::handle_read_length", (boost::format("2 bytes_transferred is expected but %1%") % bytes_transferred).str());
		handle_on_disconnected(disconnected_from::read_length);
		close();
		return;
	}

	if (!read_data_async())
	{
		read_length_async();
	}
}

void service_net_object::handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if (ec.value() != boost::system::errc::success)
	{
		error_log("service_net_object::handle_read_data", ec);
		handle_on_disconnected(disconnected_from::read_data);
		close();

		return;
	}

	read_buffer_.recieve_forward((int)bytes_transferred);
	
	if (read_buffer_.recieve_completed() == false)
	{
		read_data_async();

		return;
	}

	handle_on_read(read_buffer_);

	read_length_async();
}

void service_net_object::handle_write(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if (ec.value() != boost::system::errc::success || bytes_transferred == 0)
	{
		error_log("service_net_object::handle_write", ec);
		handle_on_disconnected(disconnected_from::write);
		close();

		return;
	}

	write_buffer_.sent((int)bytes_transferred);

	if (write_buffer_.send_completed() == false)
	{
		write_async(write_buffer_);

		return;
	}

	handle_on_write(write_buffer_);
}

void service_net_object::handle_on_disconnected(disconnected_from from)
{
	boost::system::error_code ec;
	auto ep = socket_.remote_endpoint(ec);

	if (ec.value() == boost::system::errc::success)
	{
		thread_log("disconnected: %s::%d", ep.address().to_string().c_str(), ep.port());
	}
	else
	{
		thread_log("disconnected: error get address: %d, %s", ec.value(), ec.message().c_str());
	}

}

