#pragma once

#include "config.h"
#include <unordered_map>
#include <boost/asio.hpp>
#include <string>
#include "net_object.h"


namespace mokdong
{
	namespace common
	{
		class user;
		class room;
	}

	using namespace mokdong::common;

	namespace network
	{
		class service_session_manager;
		class service_data;

		// service_session
		class service_session : public service_net_object
		{
		private:
			service_session_manager* session_manager_;
			user* user_ = nullptr;
			room* room_ = nullptr;

		public:
			service_session(service_session_manager* session_manager, boost::asio::io_service& io_service);
			virtual ~service_session();

			virtual void handle_on_close();
			virtual void handle_on_read(tcp_buffer& buffer);
			
			virtual void write_async(service_data* sd);
			virtual void do_service(tcp_buffer& buffer);

			void set_login(user* puser) { user_ = puser; }
			user* user() { return user_; }
			void set_room(room* room) { room_ = room; }
			room* room() { return room_; }
			bool  logined() { return user_ != nullptr; }
		};

		// service_session_manager
		class service_session_manager
		{
		private:
			std::unordered_map<boost::asio::ip::tcp::socket*, service_session*> sessions_;
			boost::asio::io_service& io_service_;

		public:
			service_session_manager(boost::asio::io_service& io_service);
			service_session_manager() = delete;
			service_session_manager(const service_session_manager& rhs) = delete;

			service_session& create();
			void remove(service_session* p_session);
			void purge();
		};


		// service_server
		class service_server 
		{
		private:
			boost::asio::io_service io_service_;
			boost::asio::ip::tcp::acceptor acceptor_;
			service_session_manager session_manager_;

			int port_;

		public:
			service_server(int port = default_port);

			virtual bool init();
			bool run(boost::system::error_code& ec);
			void accept_async();

			boost::asio::io_service& io_service() { return io_service_; }
			boost::asio::ip::tcp::acceptor& acceptor() { return acceptor_; }

		private:
			void handle_accept_raw(service_session* new_session, const boost::system::error_code& ec);
			virtual void handle_accept(service_session* new_session, const boost::system::error_code& ec);

		public:
			static const int default_port = 10000;
		};
	}
}