#include "room.h"
#include "type.h"
#include "user.h"
#include "service_data.h"
#include "util.h"

using namespace mokdong::common;

// room
room::room(room_manager* manager, string pname, bool plobby) 
	: manager_(manager), name_(pname), lobby_(plobby) 
{
}

void room::join(service_session& session, room_error_code& error_code)
{
	auto _user = session.user();

	if (_user == nullptr)
	{
		error_code = room_error_code::user_not_login;
		return;
	}

	auto _room = session.room();

	if (_room != nullptr && _room == this)
	{
		error_code = room_error_code::already_joined;
		return;
	}

	if (_room == nullptr)
	{
		error_code = room_error_code::success;
		joined_[_user->id()] = &session;
		session.set_room(this);
		return;
	}
	
	if (!_room->lobby())
	{
		_room->leave(session);
	}

	_room->leave(session);

	error_code = room_error_code::success;
	joined_[_user->id()] = &session;
	session.set_room(this);

	sd_room_user_join notify;
	notify.set_name(_user->id());
	broadcast(&notify, _user->id());
}

void room::leave(service_session& session, bool to_lobby)
{
	bool from_lobby = this->lobby();
	auto _user = session.user();
	
	if (_user == nullptr)
	{
		return;
	}

	joined_.erase(_user->id());

	session.set_room(nullptr);
	if (!from_lobby)
	{
		if (to_lobby)
		{
			room_error_code ec;
			manager_->lobby()->join(session, ec);
		}
	}

	sd_room_user_leave notify;
	notify.set_name(_user->id());
	broadcast(&notify);
}

void room::say(service_session& session, string str)
{
	if (str.empty()) return;
	if (this->lobby()) return;
	
	auto _user = session.user();
	if (_user == nullptr) return;

	sd_room_say_to sd;
	sd.set_from(_user->id());
	sd.set_say(str);

	broadcast(&sd);
}

void room::broadcast(service_data* data)
{
	for (auto& iter : joined_)
	{
		iter.second->write_async(data);
	}
}

void room::broadcast(service_data* data, string except)
{
	for (auto& iter : joined_)
	{
		if (iter.first != except)
		{
			iter.second->write_async(data);
		}
	}
}

void room::members(vector<string>& out)
{
	for (auto& iter : joined_)
	{
		out.push_back(iter.first);
	}
}


// room_manager
room_manager::room_manager()
{
	room* _room = new room(this, room::lobby_name, true);
	rooms_[room::lobby_name] = _room;
}

room_manager::~room_manager()
{
	// todo how handle existing room ??
	room* _room = rooms_[room::lobby_name];
	if (_room != nullptr)
	{
		rooms_.erase(room::lobby_name);
		delete _room;
	}
}

room* room_manager::create(string name, room_manager_error_code& error_code)
{
	trim(name);

	error_code = room_manager_error_code::success;

	if (rooms_[name] != nullptr)
	{
		error_code = room_manager_error_code::room_exist;

		return nullptr;
	}

	room* _room = new room(this, name);
	rooms_[name] = _room;

	return _room;
}

void room_manager::erase(string name, room_manager_error_code& error_code)
{
	trim(name);

	auto _room = rooms_[name];

	if ( _room == nullptr)
	{
		error_code = room_manager_error_code::success;
		return;
	}

	if (!_room->empty())
	{
		error_code = room_manager_error_code::room_has_members;
		return;
	}

	rooms_.erase(name);
	
	delete _room;
}

room* room_manager::get(string name, room_manager_error_code& error_code)
{
	trim(name);

	auto _room = rooms_[name];

	if (_room == nullptr)
	{
		error_code = room_manager_error_code::room_no_exist;
	}
	else
	{
		error_code = room_manager_error_code::success;
	}

	return _room;
}

void room_manager::room_names(vector<string>& out_names)
{
	out_names.clear();

	for (auto entry : rooms_)
	{
		if (!entry.second->lobby())
		{
			out_names.push_back(entry.first);
		}
	}
}

room* room_manager::lobby()
{
	return rooms_[room::lobby_name];
}

void room_manager::purge()
{
	auto iter = rooms_.begin();
	while (iter != rooms_.end())
	{
		if (!iter->second->lobby() &&  iter->second->empty())
		{
			iter = rooms_.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void room_manager::purge(string name)
{
	trim(name);
	auto _room = rooms_[name];

	if (_room == nullptr) return;
	if (!_room->lobby() && _room->empty())
	{
		room_manager_error_code ec;
		erase(name, ec);
	}
}



