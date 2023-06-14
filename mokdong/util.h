#pragma once

#include "config.h"
#include <string>
#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/format.hpp>
#include <thread>
#include <cctype>

namespace mokdong
{
	std::string format_string(const std::string fmt, ...);

	template<typename ... arguments>
	void log(const std::string fmt, arguments ... args)
	{
		std::string str = format_string(fmt, args ...);

		std::cout << str << std::endl;
	}

	template<typename ... arguments>
	void thread_log(const std::string fmt, arguments ... args)
	{
		std::string suffix = format_string("thread[%d]: ", std::this_thread::get_id());
		std::string str = format_string(fmt, args ...);

		std::cout << suffix << str << std::endl;
	}

	template<typename ... arguments>
	void dev_log(const std::string fmt, arguments ... args)
	{
		std::cout << "[dev]" << std::endl;
		thread_log(fmt, args);
	}

	inline void error_log(std::string where, const boost::system::error_code& ec)
	{
		thread_log("[error] %s: %d, %s", where.c_str(), ec.value(), ec.message().c_str());
	}
	
	inline 	void error_log(std::string where, std::string reason)
	{
		thread_log("[error] %s: %s", where.c_str(), reason.c_str());
	}

	/* string trim
	* https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
	*/
	static inline void ltrim(std::string& s) 
	{
		s.erase(
			s.begin(), 
			std::find_if(
				s.begin(), 
				s.end(), 
				[](unsigned char ch) 
				{
					return !std::isspace(ch);
				}
			)
		);
	}

	static inline void rtrim(std::string& s) 
	{
		s.erase(
			std::find_if(
				s.rbegin(), 
				s.rend(), 
				[](unsigned char ch) 
				{
					return !std::isspace(ch);
				}
			).base(), 
			s.end()
		);
	}

	static inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}
}