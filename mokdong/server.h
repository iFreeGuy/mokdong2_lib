#pragma once

#include "config.h"
#include <unordered_map>
#include <boost/asio.hpp>
#include "net_object.h"


namespace mokdong
{
	namespace network
	{
		class session_manager;


		// session
		class session : public net_object
		{
		private:
			session_manager* session_manager_;

		public:
			session(session_manager* session_manager, boost::asio::io_service& io_service);

		protected:
			virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
			virtual void handle_write(const boost::system::error_code& error);
		};


		// session manager
		class session_manager
		{
		private:
			std::unordered_map<boost::asio::ip::tcp::socket*,  session*> sessions_;
			boost::asio::io_service& io_service_;

		public:
			session_manager(boost::asio::io_service& io_service);
			session_manager() = delete;
			session_manager(const session_manager& rhs) = delete;

			session& create();
			void remove(session* p_session);
		};


		// server
		class server
		{
		private:
			boost::asio::io_service io_service_;
			boost::asio::ip::tcp::acceptor acceptor_;
			session_manager session_manager_;

			int port_;

		public:
			server(int port = default_port);

			virtual bool init();
			void run();
			void accept_async();

			boost::asio::io_service& io_service() { return io_service_; }
			boost::asio::ip::tcp::acceptor& acceptor() { return acceptor_; }

		private:
			void handle_accept_raw(session* new_session, const boost::system::error_code& error);
			virtual void handle_accept(session* new_session, const boost::system::error_code& error);

		public:
			static const int default_port = 10000;
		};
	}
}