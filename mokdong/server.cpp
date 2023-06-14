#include "server.h"
#include "net_util.h"
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include "util.h"
#include <thread>

using namespace mokdong::network;

// session
session::session(session_manager* session_manager, boost::asio::io_service& io_service)
	: net_object(io_service),
	session_manager_(session_manager)
{
}

void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		session_manager_->remove(this);
		return;
	}

	thread_log("read: %s", buffer_);

	write_async(buffer_, bytes_transferred);
}

void session::handle_write(const boost::system::error_code& error)
{
	if (error)
	{
		session_manager_->remove(this);
		return;
	}

	thread_log("write...");

	read_async();
}


// session manager
session_manager::session_manager(boost::asio::io_service& io_service)
	: io_service_(io_service)
{
}

session& session_manager::create()
{
	session* _session = new session(this, io_service_);
	sessions_.insert(std::make_pair(&_session->socket(), _session));

	return *_session;
}

void session_manager::remove(session* p_session)
{
	session* _session = sessions_[&p_session->socket()];

	if (_session != nullptr)
	{
		sessions_.erase(&p_session->socket());
		delete _session;
	}
}


// server
server::server(int port)
	: port_(port)
	//, acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	, acceptor_(io_service_)
	, session_manager_(io_service_)
{
}

bool server::init()
{
	boost::asio::ip::address addr;
	
	if (!net_util::first_v4_192(io_service_, addr)) return false;

	boost::asio::ip::tcp::endpoint ep(addr, port_);
	acceptor_.open(ep.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(ep);
	acceptor_.listen();

	log("server(%s %d) is listenning...", ep.address().to_string().c_str(), ep.port());

	return true;
}

void server::run()
{
	accept_async();

	try
	{
		io_service_.run();
	}
	catch (std::exception& e)
	{
		std::cout << "error: " << e.what() << std::endl;
	}
}

void server::accept_async()
{
	session& _session = session_manager_.create();
	acceptor_.async_accept(_session.socket(),
		boost::bind(&server::handle_accept_raw, this, &_session, boost::asio::placeholders::error)
	);
}

void server::handle_accept_raw(session* new_session, const boost::system::error_code& error)
{
	if (error)
	{
		delete new_session;

		return;
	}

	auto address = new_session->socket().remote_endpoint().address();
	mokdong::thread_log("connected: %s %d", 
		address.to_string().c_str(), 
		new_session->socket().remote_endpoint().port()
	);

	handle_accept(new_session, error);

	accept_async();
}

void server::handle_accept(session* new_session, const boost::system::error_code& error)
{
	new_session->read_async();
}
