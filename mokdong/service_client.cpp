#include "service_client.h"
#include "service_data.h"
#include "util.h"

using namespace mokdong::network;


service_client::service_client(boost::asio::io_service& io_service)
	: service_net_object(io_service)
{
}

bool service_client::connect(string ip, int port)
{
	ip::tcp::endpoint endpoint(ip::address::from_string(ip), port);
	boost::system::error_code ec;

	socket().connect(endpoint, ec);

	if (ec == boost::system::errc::success)
	{
		read_length_async();
		return true;
	}

	error_log("service_client::connect", ec);

	return false;
}

void service_client::poll()
{
	io_service().poll();
}

void service_client::handle_on_read(tcp_buffer& buffer)
{
	do_service(buffer);
}

void service_client::write_async(service_data* sd)
{
	sd->marshal(write_buffer_);
	service_net_object::write_async(write_buffer_);
}

