#pragma once

#include "config.h"
#include <boost/asio.hpp>
#include <string>
#include <vector>

namespace mokdong
{
	namespace network
	{
#define htonll(x) ((1==htonl(1)) ? (x) : (((uint64_t)htonl((x) & 0xFFFFFFFFUL)) << 32) | htonl((uint32_t)((x) >> 32)))
#define ntohll(x) ((1==ntohl(1)) ? (x) : (((uint64_t)ntohl((x) & 0xFFFFFFFFUL)) << 32) | ntohl((uint32_t)((x) >> 32)))
		

		enum class net_error_code : int
		{
			ok = 0,	
			no_ip
		};

		class net_util
		{
		public:
			static boost::asio::ip::address firstMyIP(net_error_code& err);
			static std::vector<boost::asio::ip::address> myIPs(net_error_code& err);
			static void myIPs_v4(boost::asio::io_service& io_service, std::vector<boost::asio::ip::address>& out);
			static bool first_v4_192(boost::asio::io_service& io_service, boost::asio::ip::address& out_addr);
		};
	}
}