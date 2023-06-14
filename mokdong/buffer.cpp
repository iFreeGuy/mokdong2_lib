#include "buffer.h"
#include <memory>
#include <cassert>
#include <boost/asio.hpp>
#include "net_util.h"

using namespace mokdong;


// simple_buffer

simple_buffer::simple_buffer(const char* buffer)
{
	size_t length = strlen(buffer);

	allocate(length + 1);
	memcpy(buffer_, buffer, length);
	buffer_[length] = 0;
	size_ = length + 1;
}

simple_buffer::simple_buffer(char* buffer, size_t size)
	: buffer_(buffer),
	size_(size),
	my_allocated_(false)
{
}

simple_buffer::simple_buffer(unsigned char* buffer, size_t size)
	: buffer_((char*)buffer),
	size_(size),
	my_allocated_(false)
{
}

simple_buffer::~simple_buffer()
{
	if (my_allocated_)
	{
		delete[] buffer_;
		buffer_ = 0;
		size_ = 0;
	}
}

void simple_buffer::allocate(size_t size)
{
	clear();

	buffer_ = new char[size];
	memset(buffer_, 0, size);
	my_allocated_ = true;
}

void simple_buffer::clear()
{
	if (my_allocated_)
	{
		delete[] buffer_;
	}

	buffer_ = 0;
	size_ = 0;
	my_allocated_ = false;
}

bool simple_buffer::operator== (const char* rhs)
{
	size_t length = strlen(rhs);
	if (length != size_)
	{
		return false;
	}

	return (strncmp(buffer_, rhs, size_) == 0);
}

simple_buffer& simple_buffer::operator= (const char* rhs)
{
	size_t length = strlen(rhs);

	allocate(length + 1);
	memcpy(buffer_, rhs, length);
	buffer_[length] = 0;
	size_ = length + 1;

	return *this;
}


// memory_buffer

memory_buffer::memory_buffer(int size)
	: size_(size)
{
	buffer_ = new char[size];
	last_pos_ = buffer_ + size - 1;
	cur_pos_ = buffer_;

	clear();
}

memory_buffer::memory_buffer(const memory_buffer& rhs)
{
	if (buffer_ != nullptr)
	{
		delete buffer_;
	}

	buffer_ = new char[rhs.size_];
	::memcpy(buffer_, rhs.buffer_, rhs.size_);
	last_pos_ = rhs.last_pos_;
	cur_pos_ = rhs.cur_pos_;
}

memory_buffer::memory_buffer(memory_buffer&& rhs)
{
	if (buffer_ != nullptr)
	{
		delete buffer_;
	}

	buffer_ = rhs.buffer_;
	last_pos_ = rhs.last_pos_;
	cur_pos_ = rhs.cur_pos_;
}

memory_buffer::~memory_buffer()
{
	if (buffer_ != nullptr)
	{
		delete buffer_;
		buffer_ = nullptr;
	}
}

char* memory_buffer::raw(position_t pos)
{
	assert(pos < size_);

	return buffer_ + pos;
}

const char* memory_buffer::c_raw(position_t pos) const
{
	assert(pos < size_);

	return (const char *)(buffer_ + pos);
}

void memory_buffer::position(position_t pos)
{
	assert(buffer_ + pos <= last_pos_);

	cur_pos_ = buffer_ + pos;
}

void memory_buffer::move_position(position_t pos)
{
	assert(buffer_ <= cur_pos_ + pos && cur_pos_ + pos <= last_pos_);

	cur_pos_ += pos;
}

void memory_buffer::zero(position_t start_pos)
{
	assert(buffer_ + start_pos <= last_pos_);

	::memset(buffer_ + start_pos, 0, size_ - start_pos);
}

void memory_buffer::clear()
{
	zero(0);
	cur_pos_ = 0;
}

void memory_buffer::push(const char* buffer, size_t size)
{
	assert(cur_pos_ + size - 1 <= last_pos_);

	memcpy(cur_pos_, buffer, size);
	cur_pos_ += size;
}

void memory_buffer::pop(char* buffer, size_t size)
{
	assert(cur_pos_ + size - 1 <= last_pos_);

	memcpy(buffer, cur_pos_, size);
	cur_pos_ += size;
}

void memory_buffer::peek(char* buffer, size_t size)
{
	assert(cur_pos_ + size - 1 <= last_pos_);

	memcpy(buffer, cur_pos_, size);
}

void memory_buffer::get(char* buffer, size_t size, size_t pos)
{
	assert(buffer_ + pos + size - 1 <= last_pos_);

	memcpy(buffer, buffer_ + pos, size);
}

void memory_buffer::set(char* buffer, size_t size, size_t pos)
{
	assert(buffer_ + pos + size - 1 <= last_pos_ && 0 <= pos);

	memcpy(buffer_ + pos, buffer, size);
}

char& memory_buffer::operator [](position_t index)
{
	assert(index < size_);

	return *(buffer_ + index);
}

const char& memory_buffer::operator [] (position_t index) const
{
	assert(index < size_);

	return *(buffer_ + index);
}

memory_buffer& memory_buffer::operator = (const char* rhs)
{
	size_t size = strlen(rhs);
	push(rhs, size);

	return *this;
}


// buffer_stream
buffer_stream::buffer_stream(int size)
	: memory_buffer(size)
{
}

buffer_stream::buffer_stream(const buffer_stream& rhs)
	: memory_buffer(rhs)
	, byte_order_(rhs.byte_order_)
{
}

buffer_stream::buffer_stream(buffer_stream&& rhs)
	: memory_buffer(rhs)
	, byte_order_(rhs.byte_order_)
{
}

buffer_stream& buffer_stream::push(uint8_t data)
{
	memory_buffer::push((char*)&data, 1);

	return *this;
}

buffer_stream& buffer_stream::push(int8_t data)
{
	memory_buffer::push((char *)&data, 1);

	return *this;
}

buffer_stream& buffer_stream::push(uint16_t data)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::push((char*)&data, sizeof(uint16_t));
		break;

	case byte_order::network_order:
		uint16_t ndata = htons(data);
		memory_buffer::push((char*)&ndata, sizeof(uint16_t));
		break;
	}

	return *this;
}

buffer_stream& buffer_stream::push(int16_t data)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::push((char*)&data, sizeof(int16_t));
		break;

	case byte_order::network_order:
		int16_t ndata = htons(data);
		memory_buffer::push((char*)&ndata, sizeof(int16_t));
		break;
	}

	return *this;
}

buffer_stream& buffer_stream::push(uint32_t data)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::push((char*)&data, sizeof(uint32_t));
		break;

	case byte_order::network_order:
		uint32_t ndata = htonl(data);
		memory_buffer::push((char*)&ndata, sizeof(uint32_t));
		break;
	}

	return *this;
}

buffer_stream& buffer_stream::push(int32_t data)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::push((char*)&data, sizeof(int32_t));
		break;

	case byte_order::network_order:
		int32_t ndata = htonl(data);
		memory_buffer::push((char*)&ndata, sizeof(int32_t));
		break;
	}

	return *this;
}

buffer_stream& buffer_stream::push(uint64_t data)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::push((char*)&data, sizeof(uint64_t));
		break;

	case byte_order::network_order:
		int64_t ndata = htonll(data);
		memory_buffer::push((char*)&ndata, sizeof(uint64_t));
		break;
	}

	return *this;
}

buffer_stream& buffer_stream::push(int64_t data)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::push((char*)&data, sizeof(int64_t));
		break;

	case byte_order::network_order:
		int64_t ndata = htonll(data);
		memory_buffer::push((char*)&ndata, sizeof(int64_t));
		break;
	}

	return *this;
}

buffer_stream& buffer_stream::push(simple_buffer data)
{
	push((uint16_t&)data.size_);
	memory_buffer::push(data.buffer_, data.size_);

	return *this;
}

buffer_stream& buffer_stream::push(const std::string& data)
{
	uint16_t size = (uint16_t)data.size() + 1;
	push(size);
	memory_buffer::push(data.c_str(), size);

	return *this;
}

buffer_stream& buffer_stream::push(float data)
{
	memory_buffer::push((char*)&data, sizeof(float));

	return *this;
}

buffer_stream& buffer_stream::pop(uint8_t& data)
{
	memory_buffer::pop((char*)&data, 1);

	return *this;
}

buffer_stream& buffer_stream::pop(int8_t& data)
{
	memory_buffer::pop((char*)&data, 1);

	return *this;
}

buffer_stream& buffer_stream::pop(uint16_t& data)
{
	memory_buffer::pop((char*)&data, sizeof(uint16_t));

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohs(data);
	}

	return *this;
}

buffer_stream& buffer_stream::pop(int16_t& data)
{
	memory_buffer::pop((char*)&data, sizeof(int16_t));

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohs(data);
	}

	return *this;
}

buffer_stream& buffer_stream::pop(uint32_t& data)
{
	memory_buffer::pop((char*)&data, sizeof(uint32_t));

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohl(data);
	}

	return *this;
}

buffer_stream& buffer_stream::pop(int32_t& data)
{
	memory_buffer::pop((char*)&data, sizeof(int32_t));

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohl(data);
	}

	return *this;
}

buffer_stream& buffer_stream::pop(uint64_t& data)
{
	memory_buffer::pop((char*)&data, sizeof(uint64_t));

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohll(data);
	}

	return *this;
}

buffer_stream& buffer_stream::pop(int64_t& data)
{
	memory_buffer::pop((char*)&data, sizeof(int64_t));

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohll(data);
	}

	return *this;
}

buffer_stream& buffer_stream::pop(simple_buffer& data)
{
	pop((uint16_t&)data.size_);

	if (data.empty())
	{
		data.allocate(data.size_ );
	}

	memory_buffer::pop(data.buffer_, data.size_);
	data.buffer_[data.size_ - 1] = 0;

	return *this;
}

buffer_stream& buffer_stream::pop(std::string& data)
{
	uint16_t size;
	pop(size);
	char* buffer = new char[size + 1];
	memset(buffer, 0, size+1);
	memory_buffer::pop(buffer, size);
	data = buffer;
	delete[] buffer;

	return *this;
}

buffer_stream& buffer_stream::pop(float& data)
{
	memory_buffer::pop((char*)&data, sizeof(float));

	return *this;
}

void buffer_stream::set(int16_t data, position_t position)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::set((char*)&data, sizeof(int16_t), position);
		break;

	case byte_order::network_order:
		int16_t ndata = htons(data);
		memory_buffer::set((char*)&ndata, sizeof(int16_t), position);
		break;
	}
}

void buffer_stream::set(uint16_t data, position_t position)
{
	set((int16_t)data, position);
}

void buffer_stream::set(int32_t data, position_t position)
{
	switch (byte_order_)
	{
	case byte_order::host_order:
		memory_buffer::set((char*)&data, sizeof(int32_t), position);
		break;

	case byte_order::network_order:
		int32_t ndata = htonl(data);
		memory_buffer::set((char*)&ndata, sizeof(int32_t), position);
		break;
	}
}

void buffer_stream::set(uint32_t data, position_t position)
{
	set((int32_t)data, position);
}

void buffer_stream::get(int16_t& data, position_t position)
{
	memory_buffer::get((char*)&data, sizeof(int16_t), position);

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohs(data);
	}
}

void buffer_stream::get(uint16_t& data, position_t position)
{
	int16_t temp;
	get(temp, position);

	data = (uint16_t)temp;
}


void buffer_stream::get(int32_t& data, position_t position)
{
	memory_buffer::get((char*)&data, sizeof(int32_t), position);

	if (byte_order_ == byte_order::network_order)
	{
		data = ntohl(data);
	}
}

void buffer_stream::get(uint32_t& data, position_t position)
{
	int32_t temp;
	get(temp, position);

	data = (uint32_t)temp;
}

int16_t buffer_stream::get_int16(position_t position)
{
	int16_t data;
	get(data, position);

	return data;
}

uint16_t buffer_stream::get_uint16(position_t position)
{
	return (uint16_t)get_int16(position);
}

int32_t buffer_stream::get_int32(position_t position)
{
	int32_t data;
	get(data, position);

	return data;
}

uint32_t buffer_stream::get_uint32(position_t position)
{
	return (uint32_t)get_int32(position);
}

buffer_stream& buffer_stream::operator << (uint8_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (int8_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (uint16_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (int16_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (uint32_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (int32_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (uint64_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (int64_t data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (simple_buffer& data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (const std::string& data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator << (float data)
{
	return push(data);
}

buffer_stream& buffer_stream::operator >> (uint8_t data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (int8_t data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (uint16_t& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (int16_t& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (uint32_t& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (int32_t& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (uint64_t& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (int64_t& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (simple_buffer& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (std::string& data)
{
	return pop(data);
}

buffer_stream& buffer_stream::operator >> (float& data)
{
	return pop(data);
}


// steam_service


// tcp_buffer
char* tcp_buffer::raw_length()
{
	recieved_ = 0;

	return raw(position_length);
}

char* tcp_buffer::raw_data()
{
	return raw(position_data + recieved_);
}

char* tcp_buffer::raw_send()
{
	return raw(sent_);
}

uint16_t tcp_buffer::packet_length()
{
	uint16_t data;
	get(data, position_length);

	return data;
}

uint16_t tcp_buffer::packet_service_code()
{
	uint16_t data;
	get(data, position_service_code);

	return data;
}

uint16_t tcp_buffer::recieve_remained()
{
	uint16_t result;
	result = packet_length() - recieved();

	return result;
}

uint16_t tcp_buffer::recieve_forward(int length)
{
	recieved_ += length;
	
	return recieved_;
}

void tcp_buffer::reset_tcp_info()
{
	recieved_ = 0;
	sent_ = 0;
}

uint16_t tcp_buffer::send_remained()
{
	uint16_t sent = uint16_t(size() - sent_);

	return sent;
}

bool tcp_buffer::recieve_completed()
{
	uint16_t  length = packet_length();
	
	return (length == recieved_);
}

bool tcp_buffer::send_completed()
{
	uint16_t remained = send_remained();

	return (remained == 0);
}

bool tcp_buffer::error_service_code()
{
	return (packet_service_code() == service_code_error);
}

void tcp_buffer::set_length()
{
	size_t size = memory_buffer::size();
	uint16_t sz = uint16_t(size - length_length);
	buffer_stream::set(sz, position_length);
} 

void tcp_buffer::set_session_id(int session_id)
{
	uint16_t id = static_cast<uint16_t>(session_id);
	buffer_stream::set(id, position_session_id);
}
