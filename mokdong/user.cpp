#include "user.h"
#include "cryption.h"
#include "service.h"
#include "service_data.h"
#include "room.h"
#include "global.h"

using namespace mokdong::common;
using namespace mokdong;

void user_manager::login(service_session& session, string  id, string password, user_manager_error_code& error_code)
{
	string user_password = fake_password(id);

	if (password != user_password)
	{
		error_code = user_manager_error_code::invalid_password;

		return;
	}

	user* _user = new user(id);

	logined[_user->id()] = &session;
	session.set_login(_user);

	room_error_code ec;
	room* _room = application.room_manager().lobby();
	_room->join(session, ec);

	error_code = user_manager_error_code::success;
}

void user_manager::unlogin(service_session& session)
{
	if (!session.logined()) return;

	user* _user = session.user();
	session.set_login(nullptr);
	logined.erase(_user->id());
	delete _user;
}

string user_manager::fake_password(string id)
{
	char buffer[1024];

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, id.c_str(), id.size());

	simple_cryption::encrypt(buffer, buffer + id.size());

	return string(buffer);
}
