#pragma once

#include "config.h"
#include <boost/asio.hpp>
#include "buffer.h"
#include <string>

using namespace mokdong;

namespace mokdong
{
	namespace network
	{
		class net_object
		{
		private:
			boost::asio::ip::tcp::socket socket_;
			boost::asio::io_service& io_service_;

			static const int max_length = 1024;


		protected:
			char buffer_[max_length] = "\0";


		public:
			net_object(boost::asio::io_service& io_service);

			boost::asio::ip::tcp::socket& socket() { return socket_; }
			boost::asio::io_service& io_service() { return io_service_;  }

			void read_async();
			void write_async(const char* buffer, size_t size);
			void close();

		private:
			void handle_read_raw(const boost::system::error_code& error, size_t bytes_transferred);
			void handle_write_raw(const boost::system::error_code& error);


		protected:
			virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {}
			virtual void handle_write(const boost::system::error_code& error) {}
		};

		enum class disconnected_from : int
		{
			unknown,
			read_length,
			read_data,
			write,
		};

		class service_net_object
		{
		private:
			boost::asio::ip::tcp::socket socket_;
			boost::asio::io_service& io_service_;

		protected:
			static const int max_length = 1024;
			tcp_buffer read_buffer_;
			tcp_buffer write_buffer_;

		public:
			service_net_object() = delete;
			service_net_object(boost::asio::io_service& io_service);

			boost::asio::ip::tcp::socket& socket() { return socket_; }
			boost::asio::io_service& io_service() { return io_service_; }
			bool closed() { return !socket_.is_open(); }

			void read_length_async();
			bool read_data_async();

			void write_async(tcp_buffer& buffer);

			virtual void close();

		protected:
			void handle_read_length(const boost::system::error_code& ec, size_t bytes_transferred);
			void handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred);

			void handle_write(const boost::system::error_code& ec, size_t bytes_transferred);

			virtual void handle_on_disconnected(disconnected_from from = disconnected_from::unknown);
			virtual void handle_on_close() {};
			virtual void handle_on_read(tcp_buffer& buffer) {}
			virtual void handle_on_write(tcp_buffer& buffer) {}
		};
	}
}