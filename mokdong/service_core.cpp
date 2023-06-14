#include "service_core.h"
#include "util.h"


using namespace mokdong::network;


service_executor::service_executor()
{
	register_service(new service_data_builder_maker<to_underlying(common_service_code::login), sd_login>,						
		new sh_login());
	register_service(new service_data_builder_maker<to_underlying(chat_service_code::create_room), sd_create_room>,	
		new sh_create_room());
	register_service(new service_data_builder_maker<to_underlying(chat_service_code::join_room), sd_join_room>,			
		new sh_join_room());
	register_service(new service_data_builder_maker<to_underlying(chat_service_code::get_room_list), sd_simple<to_underlying(chat_service_code::get_room_list)>>,
		new sh_get_room_list());
	register_service(new service_data_builder_maker<to_underlying(chat_service_code::leave_room), sd_simple<to_underlying(chat_service_code::leave_room)>>,
		new sh_leave_room());
	register_service(new service_data_builder_maker<to_underlying(chat_service_code::room_say), sd_room_say>,				
		new sh_room_say());
	register_service(new service_data_builder_maker<to_underlying(chat_service_code::get_room_user_list), sd_simple<to_underlying(chat_service_code::get_room_user_list)>>,
		new sh_get_room_user_list());
}

bool service_executor::do_service(tcp_buffer& buffer, service_net_object* session)
{
	auto service_code = buffer.packet_service_code();
	auto builder = builders_[service_code];

	if (builder == nullptr)
	{
		error_log("service_executor::do_service",
			(boost::format("service_data_builder, unsupported service code: %1%") % service_code).str());

		return false;
	}

	auto data = builder->create();
	data->demarshal(buffer);

	auto result = do_service(data, session);
	delete data;

	return result;
}

bool service_executor::do_service(service_data* data, service_net_object* session)
{
	auto service_code = data->service_code();
	auto handler = handlers_[service_code];

	if (handler == nullptr)
	{
		error_log("service_executor::do_service",
			(boost::format("service_handler, unsupported service code: %1%") % service_code).str());
	
		return false;
	}

	handler->do_service(data, session);

	return true;
}

void service_executor::register_service(service_data_builder* builder, service_handler* handler)
{
	builders_[builder->service_code()] = builder;
	handlers_[handler->service_code()] = handler;
}

void service_executor::unregister_service_builder(uint16_t service_code)
{
	auto builder = builders_[service_code];
	if (builder != nullptr)
	{
		delete builder;
	}
}

void service_executor::unregister_service_handler(uint16_t service_code)
{
	auto handler = handlers_[service_code];
	if (handler != nullptr)
	{
		delete handler;
	}
}

