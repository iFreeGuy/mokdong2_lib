#pragma once

#include "config.h"
#include <boost/asio.hpp>

namespace mokdong
{
	namespace common
	{
		enum class common_service_code : uint16_t
		{
			login = 1,
			login_result = 2
		};

		enum class chat_service_code : uint16_t
		{
			create_room = 201,
			create_room_result = 202,
			get_room_list = 203,
			room_list = 204,
			join_room = 205,
			join_room_result = 206,
			leave_room = 207,
			room_say = 208,
			room_say_to = 209,
			get_room_user_list = 210,
			room_user_list = 211,
			room_user_join = 212,
			room_user_leave = 213,
		};
	}
}



