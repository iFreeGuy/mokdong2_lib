#pragma once

#include "buffer.h"
#include <string>
#include "service.h"

using namespace std;

using namespace mokdong;
using namespace mokdong::common;

namespace mokdong
{
	namespace network
	{
		class service_data
		{
		public:
			void demarshal(tcp_buffer& buffer);
			void marshal(tcp_buffer& buffer);

			virtual uint16_t service_code() = 0;

		private:
			void demarshal_header(tcp_buffer& buffer);
			void marshal_header(tcp_buffer& buffer);

		protected:
			virtual void demarshal_data(tcp_buffer& buffer) = 0;
			virtual void marshal_data(tcp_buffer& buffer) = 0;
		};

		template<uint16_t SERVICE_CODE>
		class sd_simple_result : public service_data
		{
		private:
			int16_t result_;

		public:
			sd_simple_result() = default;
			sd_simple_result(int16_t result) : result_(result) {}

			virtual uint16_t service_code() { return SERVICE_CODE; }
			int16_t result() { return result_; }

			virtual void demarshal_data(tcp_buffer& buffer) 
			{
				buffer >> result_;
			}
			virtual void marshal_data(tcp_buffer& buffer)
			{
				buffer << result_;
			}
		};

		template<uint16_t SERVICE_CODE>
		class sd_simple : public service_data
		{
		public:
			sd_simple() = default;

			virtual uint16_t service_code() { return SERVICE_CODE; }

			virtual void demarshal_data(tcp_buffer& buffer) {}
			virtual void marshal_data(tcp_buffer& buffer) {}
		};

		class sd_login : public service_data
		{
		private:
			string id_;
			string password_;

		public:
			virtual uint16_t service_code() { return to_underlying(common_service_code::login); }

			void set_id(string id) { id_ = id; }
			string id() const { return id_; }

			void set_password(string password) { password_ = password; }
			string password() const { return password_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_create_room : public service_data
		{
		private:
			string name_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::create_room); }

			void set_name(string name) { name_ = name; }
			string name() { return name_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_room_list : public service_data
		{
		private:
			vector<string> names_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_list); }

			void add(string name);
			vector<string>& names() { return names_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_join_room : public service_data
		{
		private:
			string name_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::join_room); }

			void set_name(string name) { name_ = name; }
			string name() { return name_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_room_say : public service_data
		{
		private:
			string say_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_say); }

			void set_say(string say) { say_ = say; }
			string say() { return say_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_room_say_to : public service_data
		{
		private:
			string from_;
			string say_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_say_to); }

			void set_from(string id) { from_ = id; }
			string from() { return from_; }
			void set_say(string say) { say_ = say; }
			string say() { return say_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_room_user_list : public service_data
		{
		private:
			vector<string> names_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_user_list); }

			void add(string name);
			vector<string>& names() { return names_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_room_user_join : public service_data
		{
		private:
			string name_;

		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_user_join); }

			void set_name(string name) { name_ = name; }
			string name() { return name_; }

			virtual void demarshal_data(tcp_buffer& buffer);
			virtual void marshal_data(tcp_buffer& buffer);
		};

		class sd_room_user_leave : public sd_room_user_join
		{
		public:
			virtual uint16_t service_code() { return to_underlying(chat_service_code::room_user_leave); }
		};
	}
}