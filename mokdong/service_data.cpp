#pragma once

#include "service_data.h"

using namespace mokdong::network;


void service_data::demarshal(tcp_buffer& buffer)
{
	demarshal_header(buffer);
	demarshal_data(buffer);
}

void service_data::marshal(tcp_buffer& buffer)
{
	buffer.clear();
	marshal_header(buffer);
	marshal_data(buffer);
	buffer.set_length();
}

void service_data::demarshal_header(tcp_buffer& buffer)
{
	uint16_t length;
	uint16_t session_id;
	uint16_t service_code;

	buffer.to_header();
	buffer >> length;
	buffer >> session_id;
	buffer >> service_code;
}

void service_data::marshal_header(tcp_buffer& buffer)
{
	buffer.reset_tcp_info();

	uint16_t length = 0;
	uint16_t session_id = 0;
	buffer.to_header();
	buffer << length;
	buffer << session_id;
	buffer << service_code();
}


// sd_login

void sd_login::demarshal_data(tcp_buffer& buffer)
{
	buffer >> id_;
	buffer >> password_;
}

void sd_login::marshal_data(tcp_buffer& buffer)
{
	buffer <<  id_;
	buffer << password_;
}


// sd_create_room

void sd_create_room::demarshal_data(tcp_buffer& buffer)
{
	buffer >> name_;
}

void sd_create_room::marshal_data(tcp_buffer& buffer)
{
	buffer << name_;
}


// sd_room_list

void sd_room_list::add(string name)
{
	names_.push_back(name);
}

void sd_room_list::demarshal_data(tcp_buffer& buffer)
{
	uint16_t size;

	names_.clear();
	buffer >> size;

	string name;
	for (int i = 0; i < size; i++)
	{
		buffer >> name;
		add(name);
	}
}

void sd_room_list::marshal_data(tcp_buffer& buffer)
{
	buffer << (uint16_t)names_.size();

	for (auto& name : names_)
	{
		buffer << name;
	}
}


// sd_join_room

void sd_join_room::demarshal_data(tcp_buffer& buffer)
{
	buffer >> name_;
}

void sd_join_room::marshal_data(tcp_buffer& buffer)
{
	buffer << name_;
}


// sd_room_say

void sd_room_say::demarshal_data(tcp_buffer& buffer)
{
	buffer >> say_;
}

void sd_room_say::marshal_data(tcp_buffer& buffer)
{
	buffer << say_;
}


// sd_room_say_to

void sd_room_say_to::demarshal_data(tcp_buffer& buffer)
{
	buffer >> from_;
	buffer >> say_;
}

void sd_room_say_to::marshal_data(tcp_buffer& buffer)
{
	buffer << from_;
	buffer << say_;
}


// sd_room_user_list

void sd_room_user_list::add(string name)
{
	names_.push_back(name);
}

void sd_room_user_list::demarshal_data(tcp_buffer& buffer)
{
	uint16_t size;

	names_.clear();
	buffer >> size;

	string name;
	for (int i = 0; i < size; i++)
	{
		buffer >> name;
		add(name);
	}
}

void sd_room_user_list::marshal_data(tcp_buffer& buffer)
{
	buffer << (uint16_t)names_.size();

	for (auto& name : names_)
	{
		buffer << name;
	}
}


// sd_room_user_join

void sd_room_user_join::demarshal_data(tcp_buffer& buffer)
{
	buffer >> name_;
}

void sd_room_user_join::marshal_data(tcp_buffer& buffer)
{
	buffer << name_;
}






