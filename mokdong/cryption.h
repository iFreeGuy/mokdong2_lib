#pragma once

namespace mokdong
{
	class simple_cryption
	{
	private:
		const static int key = 65241;

	public:
		static void encrypt(char* start, char* end);
		static void decrypt(char* start, char* end);
	};
}