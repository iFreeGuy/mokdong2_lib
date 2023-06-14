#include "net_util.h"

using namespace mokdong::network;

boost::asio::ip::address net_util::firstMyIP(net_error_code& err)
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);

	err = net_error_code::ok;

	if (iter != boost::asio::ip::tcp::resolver::iterator())
	{
		return (iter)->endpoint().address();
	}
	
	err = net_error_code::no_ip;
	return boost::asio::ip::address();
}

std::vector<boost::asio::ip::address> net_util::myIPs(net_error_code& err)
{
	std::vector<boost::asio::ip::address> ips;

	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);

	err = net_error_code::ok;

	while (iter != boost::asio::ip::tcp::resolver::iterator())
	{
		boost::asio::ip::address addr = (iter)->endpoint().address();
		ips.emplace_back(addr);

		iter++;
	}

	return ips;
}

void net_util::myIPs_v4(boost::asio::io_service& io_service, std::vector<boost::asio::ip::address>& out)
{
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);

	while (iter != boost::asio::ip::tcp::resolver::iterator())
	{
		boost::asio::ip::address addr = (iter)->endpoint().address();
		
		if (addr.is_v4())
		{
			out.push_back(addr);
		}

		iter++;
	}
}

 bool net_util::first_v4_192(boost::asio::io_service& io_service, boost::asio::ip::address& out_addr)
{
	std::vector<boost::asio::ip::address> addrs;
	myIPs_v4(io_service, addrs);

	for (auto addr : addrs)
	{
		std::string str = addr.to_string();
		if (str.substr(0, 3) == "192")
		{
			out_addr = addr;

			return true;
		}
	}

	return false;
}








