#include "service_handler.h"
#include "user.h"
#include "global.h"
#include "service_server.h"
#include <vector>

using namespace mokdong::network;
using namespace mokdong::common;


// sh_login
void sh_login::do_service(service_data* data, service_net_object* session)
{
	sd_login* sd	 = dynamic_cast<sd_login*>(data);
	service_session* ssession	= dynamic_cast<service_session*>(session);
	
	user_manager_error_code ec;
	 application.user_manager().login(*ssession, sd->id(), sd->password(), ec);

	sd_simple_result<to_underlying(common_service_code::login_result)>  result(to_underlying(ec));
	ssession->write_async(&result);
}


// sh_create_room
void sh_create_room::do_service(service_data* data, service_net_object* session)
{
	sd_create_room* sd = dynamic_cast<sd_create_room*>(data);
	service_session* ssession = dynamic_cast<service_session*>(session);

	room_manager_error_code ec;
	auto _room = application.room_manager().create(sd->name(), ec);
	if (ec != room_manager_error_code::success)
	{
		sd_simple_result<to_underlying(chat_service_code::create_room_result)>  result(to_underlying(ec));
		ssession->write_async(&result);
		return;
	}

	room_error_code room_ec;
	_room->join(*ssession, room_ec);
	sd_simple_result<to_underlying(chat_service_code::create_room_result)>  result(to_underlying(room_ec));
	ssession->write_async(&result);
}


// sh_join_room
void sh_join_room::do_service(service_data* data, service_net_object* session)
{
	sd_join_room* sd = dynamic_cast<sd_join_room*>(data);
	service_session* ssession = dynamic_cast<service_session*>(session);

	room_manager_error_code ec;
	auto _room = application.room_manager().get(sd->name(), ec);
	if (ec != room_manager_error_code::success)
	{
		sd_simple_result<to_underlying(chat_service_code::join_room_result)>  result(to_underlying(ec));
		ssession->write_async(&result);
		return;
	}

	room_error_code room_ec;
	_room->join(*ssession, room_ec);
	sd_simple_result<to_underlying(chat_service_code::join_room_result)>  result(to_underlying(room_ec));
	ssession->write_async(&result);
}

// sh_leave_room
void sh_leave_room::do_service(service_data* data, service_net_object* session)
{
	sd_simple<to_underlying(chat_service_code::leave_room)>* sd 
		= (sd_simple<to_underlying(chat_service_code::leave_room)>*)(data);
	service_session* ssession = dynamic_cast<service_session*>(session);

	auto _room = ssession->room();
	
	if (_room == nullptr)
	{
		return;
	}

	_room->leave(*ssession);
	if (_room->empty())
	{
		application.room_manager().purge(_room->name());
	}
}

// sh_room_say
void sh_room_say::do_service(service_data* data, service_net_object* session)
{
	sd_room_say* sd = dynamic_cast<sd_room_say*>(data);
	service_session* ssession = dynamic_cast<service_session*>(session);

	auto _room = ssession->room();
	if (_room == nullptr) return;

	_room->say(*ssession, sd->say());
}


// sh_get_room_list
void sh_get_room_list::do_service(service_data* data, service_net_object* session)
{
	sd_simple<to_underlying(chat_service_code::get_room_list)>* sd = dynamic_cast<sd_simple<to_underlying(chat_service_code::get_room_list)>*>(data);
	service_session* ssession = dynamic_cast<service_session*>(session);

	sd_room_list sd_ret;
	vector<string> names;
	application.room_manager().room_names(names);

	for (auto name : names)
	{
		sd_ret.add(name);
	}

	ssession->write_async(&sd_ret);
}


// sh_get_room_user_list
void sh_get_room_user_list::do_service(service_data* data, service_net_object* session)
{
	sd_simple<to_underlying(chat_service_code::get_room_user_list)>* sd = dynamic_cast<sd_simple<to_underlying(chat_service_code::get_room_user_list)>*>(data);
	service_session* ssession = dynamic_cast<service_session*>(session);

	sd_room_user_list sd_ret;
	auto _room = ssession->room();
	if (_room == nullptr)
	{
		return;
	}

	if (_room->lobby())
	{
		return;
	}

	vector<string> names;
	_room->members(names);

	for (auto name : names)
	{
		sd_ret.add(name);
	}

	ssession->write_async(&sd_ret);
}











