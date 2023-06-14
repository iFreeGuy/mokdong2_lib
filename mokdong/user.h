#pragma once

#include"service_server.h"
#include <unordered_map>

using namespace std;
using namespace mokdong::network;

namespace mokdong
{
	namespace common
	{
		class user
		{
		private:
			string id_;

		public:
			user(string pid) : id_(pid) {}

			string id() { return id_; }
		};

		enum class user_manager_error_code : uint16_t
		{
			success = 0,
			invalid_password = 1,
		};

		class user_manager
		{
		private:
			unordered_map<string, service_session*> logined;

		public:
			void login(service_session& session, string  id, string password, user_manager_error_code& error_code);
			void unlogin(service_session& session);

		private:
			string fake_password(string id);
		};
	}
}