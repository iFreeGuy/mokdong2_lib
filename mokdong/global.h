#pragma once

#include "pattern.h"
#include "service_core.h"
#include "user.h"
#include "room.h"

using namespace mokdong::network;
using namespace mokdong::common;

namespace mokdong
{
	class global
	{
	private:
		service_executor service_executor_;
		user_manager user_manager_;
		room_manager room_manager_;

	public:
		service_executor& service_executor() { return service_executor_; }
		user_manager& user_manager() { return user_manager_;  }
		room_manager& room_manager() { return room_manager_; }

		void purge();
	};

	#define application singleton<global>::instance()
}
