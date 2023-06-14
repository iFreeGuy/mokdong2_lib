#pragma once

#include <cstddef>
#include <type_traits>

namespace mokdong
{
	typedef unsigned int position_t;

	// https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int
	template<typename E>
	constexpr auto to_underlying(E e) noexcept
	{
		return static_cast<std::underlying_type_t<E>>(e);
	}
}