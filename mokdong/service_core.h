#pragma once

#include "service_data.h"
#include "service_handler.h"
#include "buffer.h"
#include <string>
#include <map>

using namespace std;

namespace mokdong
{
	namespace network
	{
		class service_data_builder
		{
		public:
			virtual uint16_t service_code() = 0;
			virtual service_data* create() = 0;
		};

		template<uint16_t SERVICE_CODE, class TYPE>
		class service_data_builder_maker : public service_data_builder
		{
		public:
			virtual uint16_t service_code() { return SERVICE_CODE;  }
			virtual service_data* create() { return new TYPE;  }
		};

		class service_executor
		{
		private:
			map<uint16_t, service_data_builder*> builders_;
			map<uint16_t, service_handler*> handlers_;

		public:
			service_executor();

			bool do_service(tcp_buffer& buffer, service_net_object* session);
			bool do_service(service_data* data, service_net_object* session);
			void register_service(service_data_builder* builder, service_handler* handler);
			void unregister_service_builder(uint16_t service_code);
			void unregister_service_handler(uint16_t service_code);
		};
	}
}



