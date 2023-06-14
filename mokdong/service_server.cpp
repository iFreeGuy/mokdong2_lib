#include <boost/bind.hpp>
#include "service_server.h"
#include "util.h"
#include "global.h"
#include "net_util.h"


using namespace mokdong::network;

// service_session
service_session::service_session(service_session_manager* session_manager, boost::asio::io_service& io_service)
	: service_net_object(io_service), 
	session_manager_(session_manager)
{
}

service_session::~service_session()
{
	close();
}

void service_session::handle_on_close()
{
	if (logined())
	{
		if (room_ != nullptr)
		{
			string name = room_->name();
			room_->leave(*this, false);
			application.room_manager().purge(name);
		}
		application.user_manager().unlogin(*this);
	}
}

void service_session::handle_on_read(tcp_buffer& buffer)
{
	thread_log("recv: service_code: %d", buffer.packet_service_code());

	do_service(buffer);
}

void service_session::write_async(service_data* sd)
{
	thread_log("write: service_code: %d", sd->service_code());

	sd->marshal(write_buffer_);
	service_net_object::write_async(write_buffer_);
}

void service_session::do_service(tcp_buffer& buffer)
{
	application.service_executor().do_service(buffer, this);
}



// service_session_manager

service_session_manager::service_session_manager(boost::asio::io_service& io_service)
	: io_service_(io_service)
{
}

service_session& service_session_manager::create()
{
	service_session* _session = new service_session(this, io_service_);
	sessions_.insert(std::make_pair(&_session->socket(), _session));

	return *_session;
}

void service_session_manager::remove(service_session* p_session)
{
	service_session* _session = sessions_[&p_session->socket()];

	if (_session != nullptr)
	{
		sessions_.erase(&p_session->socket());
		_session->close();
		delete _session;
	}
}

void service_session_manager::purge()
{
	auto iter = sessions_.begin();
	while (iter != sessions_.end())
	{
		if (iter->second->closed())
		{
			sessions_.erase(iter++);
		}
		else
		{
			iter++;
		}
	}
}


// service_server

service_server::service_server(int port)
	: port_(port)
	, acceptor_(io_service_)
	, session_manager_(io_service_)
{
}

bool service_server::init()
{
	boost::asio::ip::address addr;

	if (!net_util::first_v4_192(io_service_, addr)) return false;

	boost::asio::ip::tcp::endpoint ep(addr, port_);
	acceptor_.open(ep.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(ep);
	acceptor_.listen();

	thread_log("service server(%s %d) is listenning...", ep.address().to_string().c_str(), ep.port());

	return true;
}

bool service_server::run(boost::system::error_code& ec)
{
	accept_async();

	io_service_.run(ec);
	
	if (ec.value() == boost::system::errc::success)
	{
		return true;
	}
	else
	{
		error_log("service_server::run", ec);
	}

	return false;
}

void service_server::accept_async()
{
	service_session& _session = session_manager_.create();
	boost::system::error_code ec;

	acceptor_.async_accept(_session.socket(),
		boost::bind(&service_server::handle_accept_raw, this, &_session, ec)
	);

	if (ec.value() != boost::system::errc::success)
	{
		error_log("service_server::accept_async", ec);
	}
}

void service_server::handle_accept_raw(service_session* new_session, const boost::system::error_code& ec)
{
	if (ec)
	{
		session_manager_.remove(new_session);
	}
	else
	{
		auto address = new_session->socket().remote_endpoint().address();
		mokdong::thread_log("connected: %s::%d",
			address.to_string().c_str(),
			new_session->socket().remote_endpoint().port()
		);

		handle_accept(new_session, ec);
	}

	session_manager_.purge();
	application.purge();

	accept_async();
}

void service_server::handle_accept(service_session* new_session, const boost::system::error_code& ec)
{
	new_session->read_length_async();
}

