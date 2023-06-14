#pragma once

#include "type.h"
#include "config.h"
#include <string>

namespace mokdong
{
	struct simple_buffer
	{
	private:
		bool my_allocated_ = false;

	public:
		char* buffer_ = 0;
		size_t size_ = 0;

		simple_buffer() = default;
		simple_buffer(const char* buffer);
		simple_buffer(char* buffer, size_t size);
		simple_buffer(unsigned char* buffer, size_t size);
		virtual ~simple_buffer();

		void allocate(size_t size);
		void clear();
		bool empty() const { return buffer_ == 0; }
		bool my_allocated() const { return my_allocated_; }

		bool operator== (const char* rhs);
		simple_buffer& operator= (const char* rhs);
	};

	typedef simple_buffer simple_string;

	class memory_buffer
	{
	public:
		static const int start = 0;

	protected:
		char* buffer_ = nullptr;
		char* cur_pos_ = nullptr;
		char* last_pos_ = nullptr;
		size_t size_ = 0;

	public:
		memory_buffer(int size);
		memory_buffer(const memory_buffer& rhs);
		memory_buffer(memory_buffer&& rhs);
		virtual ~memory_buffer();

		char* raw(position_t pos = 0);
		const char* c_raw(position_t pos = 0) const;
		void position(position_t pos);
		void move_position(position_t pos);

		void zero(position_t start_pos = 0);
		void clear();
		size_t capacity() { return size_; }
		size_t size() { return cur_pos_ - buffer_ ;  }
		size_t remains() { return (size_ - (cur_pos_ - buffer_)); }

		void push(const char* buffer, size_t size);
		void push(char* buffer, size_t size) { push(static_cast<const char*>(buffer), size); }
		void pop(char* buffer, size_t size);
		void peek(char* buffer, size_t size);
		void get(char* buffer, size_t size, size_t pos);
		void set(char* buffer, size_t size, size_t pos);

		char& operator [](position_t index);
		const char& operator [] (position_t index) const;
		memory_buffer& operator = (const char* rhs);
	};

	// buffer_stream
	class buffer_stream : public memory_buffer
	{
	public:
		enum class byte_order : int
		{
			host_order = 0,
			network_order = 1
		};

	private:
		byte_order byte_order_ = byte_order::network_order;

	public:
		buffer_stream() = delete;
		buffer_stream(int size = default_buffer_size);
		buffer_stream(const buffer_stream& rhs);
		buffer_stream(buffer_stream&& rhs);
		virtual ~buffer_stream() = default;

		buffer_stream& push(uint8_t data);
		buffer_stream& push(int8_t data);
		buffer_stream& push(uint16_t data);
		buffer_stream& push(int16_t data);
		buffer_stream& push(uint32_t data);
		buffer_stream& push(int32_t data);
		buffer_stream& push(uint64_t data);
		buffer_stream& push(int64_t data);
		buffer_stream& push(simple_buffer data);
		buffer_stream& push(const std::string& data);
		buffer_stream& push(float data);

		buffer_stream& pop(uint8_t& data);
		buffer_stream& pop(int8_t& data);
		buffer_stream& pop(uint16_t & data);
		buffer_stream& pop(int16_t& data);
		buffer_stream& pop(uint32_t& data);
		buffer_stream& pop(int32_t& data);
		buffer_stream& pop(uint64_t& data);
		buffer_stream& pop(int64_t& data);
		buffer_stream& pop(simple_buffer& data);
		buffer_stream& pop(std::string& data);
		buffer_stream& pop(float& data);

		void set(int16_t data, position_t position);
		void set(uint16_t data, position_t position);
		void set(int32_t data, position_t position);
		void set(uint32_t data, position_t position);
		void get(int16_t& data, position_t position);
		void get(uint16_t& data, position_t position);
		void get(int32_t& data, position_t position);
		void get(uint32_t& data, position_t position);
		int16_t get_int16(position_t position);
		uint16_t get_uint16(position_t position);
		int32_t get_int32(position_t position);
		uint32_t get_uint32(position_t position);

		buffer_stream& operator << (uint8_t data);
		buffer_stream& operator << (int8_t data);
		buffer_stream& operator << (uint16_t data);
		buffer_stream& operator << (int16_t data);
		buffer_stream& operator << (uint32_t data);
		buffer_stream& operator << (int32_t data);
		buffer_stream& operator << (uint64_t data);
		buffer_stream& operator << (int64_t data);
		buffer_stream& operator << (simple_buffer& data);
		buffer_stream& operator << (const std::string& data);
		buffer_stream& operator << (float data);

		buffer_stream& operator >> (uint8_t data);
		buffer_stream& operator >> (int8_t data);
		buffer_stream& operator >> (uint16_t& data);
		buffer_stream& operator >> (int16_t& data);
		buffer_stream& operator >> (uint32_t& data);
		buffer_stream& operator >> (int32_t& data);
		buffer_stream& operator >> (uint64_t& data);
		buffer_stream& operator >> (int64_t& data);
		buffer_stream& operator >> (simple_buffer& data);
		buffer_stream& operator >> (std::string& data);
		buffer_stream& operator >> (float& data);
	};


	// buffer_stream_service
	class stream_service : public buffer_stream
	{
	public:
		stream_service() = delete;
		stream_service(int size = default_buffer_size) : buffer_stream(size) {}
		stream_service(const stream_service& rhs) : buffer_stream(rhs) {}
		virtual ~stream_service() = default;

		virtual int16_t service_code() = 0;
	};


	// tcp_buffer
	class tcp_buffer : public buffer_stream
	{
	private:
		int recieved_ = 0;
		int sent_ = 0;

	public:
		static const int position_length = 0;
		static const int position_header = 0;
		static const int position_data = 2;
		static const int position_session_id= 2;
		static const int position_service_code = 4;
		static const int service_code_error = 999;
		static const int length_length = 2;

		tcp_buffer(int size = default_buffer_size) : buffer_stream(size) {}
		virtual ~tcp_buffer() = default;

		void to_length() { position(position_length); }
		void to_header() { position(position_header); }
		void to_data() { position(position_data); }

		char* raw_length();
		char* raw_data();
		char* raw_send();

		uint16_t packet_length();
		uint16_t packet_service_code();

		uint16_t recieved() { return recieved_;  }
		uint16_t recieve_remained();
		uint16_t recieve_forward(int length);
		
		void reset_tcp_info();

		void sent(int length) { sent_ += length; }
		uint16_t send_remained();

		bool recieve_completed();
		bool send_completed();
		bool error_service_code();

		void set_length();
		void set_session_id(int session_id);
	};
}




