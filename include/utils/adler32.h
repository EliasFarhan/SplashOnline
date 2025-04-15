#pragma once

#include <cstdint>
#include <ranges>
#include <span>

#include <math/vec2.h>
#include "engine/input_manager.h"
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
		return Add(static_cast<float>(s));
	}

	template<std::ranges::range R>
	uint32_t Add(R&& range)
	{
	    uint32_t result = 0;
	    for (auto&& elem: range)
	    {
	        result = Add(elem);
	    }
	    return result;
	}


    uint32_t Add(const PlayerInput& playerInput)
	{
	    Add(playerInput.moveDirX);
	    Add(playerInput.moveDirY);
	    Add(playerInput.targetDirX);
	    Add(playerInput.targetDirY);
	    return Add(playerInput.buttons);
	}

	template<typename T>
    requires !std::ranges::range<T>
	uint32_t Add(const T& data)
	{
		if constexpr (sizeof(T) == sizeof(uint32_t))
		{
			a = (a + std::bit_cast <uint32_t>(data)) % MOD_ADLER;
			b = (b + a) % MOD_ADLER;
			return (b << 16) | a;
		}
		else if constexpr (sizeof(T) == sizeof (uint16_t))
		{
			a = (a + std::bit_cast <uint16_t>(data)) % MOD_ADLER;
			b = (b + a) % MOD_ADLER;
			return (b << 16) | a;
		}
		else if constexpr (sizeof(T) == sizeof (uint8_t))
		{
			a = (a + std::bit_cast <uint8_t>(data)) % MOD_ADLER;
			b = (b + a) % MOD_ADLER;
			return (b << 16) | a;
		}
		else if constexpr (sizeof (T) == sizeof(uint64_t))
		{
			const auto tmpValue = std::bit_cast <uint64_t>(data);
			const auto lowerValue = static_cast<uint32_t>(tmpValue);
			const auto upperValue = static_cast<uint32_t>(tmpValue>>32);
			Add(lowerValue);
			return Add(upperValue);
		}
		else
		{
		    assert(false && "Don't use please");
			return Add(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
		}
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