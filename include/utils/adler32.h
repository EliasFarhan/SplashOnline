#pragma once

#include <cstdint>
#include <ranges>

#include <math/vec2.h>

#include "timer.h"

namespace splash
{

class Adler32
{
	uint32_t a = 1, b = 0;
	static constexpr uint32_t MOD_ADLER = 65521;
public:
	void Reset()
	{
		a = 1;
		b = 0;
	}
	[[nodiscard]] uint32_t GetValue() const
	{
		return (b << 16) | a;
	}

	template<typename T=neko::Scalar, int Period=0, int Dividend=1000>
	uint32_t Add(const Timer<T, Period, Dividend>& timer)
	{
		return Add(timer.RemainingTime());
	}

	template<class T>
	uint32_t Add(neko::Vec2<T> v)
	{
		Add(v.x);
		return Add(v.y);
	}

	uint32_t Add(neko::Scalar s)
	{
		return Add(s.to_float());
	}

	template<class T>
	uint32_t Add(std::span<T> range)
	{
		uint32_t result = 0;
		for (auto& elem: range)
		{
			result = Add(elem);
		}
		return result;
	}


	template<typename T>
	uint32_t Add(const T& data)
	{
		return Add(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
	}
	uint32_t Add(const uint8_t* data, size_t len)
	{
		// Process each byte of the data in order
		for (size_t index = 0; index < len; ++index)
		{
			a = (a + data[index]) % MOD_ADLER;
			b = (b + a) % MOD_ADLER;
		}

		return (b << 16) | a;
	}
};

}