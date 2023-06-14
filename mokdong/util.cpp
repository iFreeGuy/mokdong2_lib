#include "util.h"
#include <stdarg.h>
#include <cassert>
#include <iostream>

// base: https://wendys.tistory.com/116


std::string mokdong::format_string(const std::string fmt, ...)
{
	int size = ((int)fmt.size()) * 2;
	std::string buffer;

	va_list ap;
	while (true)
	{
		buffer.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char*)buffer.data(), size, fmt.c_str(), ap);
		va_end(ap);

		if (-1 < n && n < size)
		{
			buffer.resize(n);
			return buffer;
		}
		else
		{
			size *= 2;
		}
	}

	assert(true);

	return buffer;
}

