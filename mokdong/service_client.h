#pragma once

#include "config.h"
#include <boost/asio.hpp>
#include "net_object.h"
#include <string>

using namespace std;
using namespace boost::asio;

namespace mokdong
{
	namespace network
	{
		class service_data;

		class service_client : public service_net_object
		{
		public:
			service_client(boost::asio::io_service& io_service);
			virtual ~service_client() = default;

			bool connect(string ip, int port);
			void poll();

			virtual void handle_on_read(tcp_buffer& buffer);
			virtual void write_async(service_data* sd);
			virtual void do_service(tcp_buffer& buffer) {}
		};
	}
}