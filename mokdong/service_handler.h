#pragma once

#include "service_data.h"
#include "net_object.h"
#include "service.h"

namespace mokdong
{
	namespace network
	{
		class service_handler
		{
		public:
			virtual uint16_t service_code() = 0;
			virtual void do_service(service_data* data, service_net_object* session) = 0;
		};

		class sh_login : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(common_service_code::login); }
			virtual void do_service(service_data* data, service_net_object* session);
		};

		class sh_create_room : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::create_room); }
			virtual void do_service(service_data* data, service_net_object* session);
		};

		class sh_join_room : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::join_room); }
			virtual void do_service(service_data* data, service_net_object* session);
		};

		class sh_leave_room : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::leave_room); }
			virtual void do_service(service_data* data, service_net_object* session);
		};

		class sh_room_say : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_say); }
			virtual void do_service(service_data* data, service_net_object* session);
		};

		class sh_get_room_list : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::get_room_list); }
			virtual void do_service(service_data* data, service_net_object* session);
		};

		class sh_get_room_user_list : public service_handler
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::get_room_user_list); }
			virtual void do_service(service_data* data, service_net_object* session);
		};
	}
}