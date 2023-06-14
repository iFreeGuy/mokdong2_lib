#pragma once

#include <unordered_map>
#include <vector>
#include "service_server.h"
#include "buffer.h"

using namespace std;
using namespace mokdong::network;

namespace mokdong
{
	namespace network
	{
		class service_data;
	}
}

namespace mokdong
{
	namespace common
	{
		class room_manager;

		enum class room_error_code : int
		{
			success = 0,
			user_not_login = 1,
			already_joined = 2,
		};

		class room
		{
		private:
			unordered_map<string, service_session*> joined_;
			string name_;
			bool lobby_ = false;
			room_manager* manager_;


		public:
			room(room_manager* manager, string pname, bool plobby = false);

			void join(service_session& session, room_error_code& error_code );
			void leave(service_session& session, bool to_lobby = true);

			void say(service_session& session, string str);
			void broadcast(service_data* data);
			void broadcast(service_data* data, string except);

			string name() { return name_; }
			bool lobby() { return lobby_; }
			int count_members() { return (int)joined_.size(); }
			int empty() { return (0 == joined_.size()); }
			void members(vector<string>& out);

			static constexpr const char* lobby_name = "lobby";
		};

		/* sometimes, it is possible to use with room_error_code.
		*  so, room_manager_error_code is started from 100.
		*/
		enum class room_manager_error_code : int
		{
			success = 0,
			room_exist = 100,
			room_has_members = 101,
			room_no_exist = 102,
		};

		class room_manager
		{
		private:
			unordered_map<string, room*> rooms_;

		public:
			room_manager();
			virtual ~room_manager();

			room* create(string name, room_manager_error_code& error_code);
			void erase(string name, room_manager_error_code& error_code);
			room* get(string name, room_manager_error_code& error_code);
			void room_names(vector<string>& out_names);

			room* lobby();
			void purge();
			void purge(string name);
		};
	}
}