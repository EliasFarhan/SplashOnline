#pragma once

#include <cstdint>

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

	template<typename T>
	uint32_t Add(const T& data)
	{
		return Add(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
	}
	uint32_t Add(const uint8_t* data, size_t len)
	{
		size_t index;

		// Process each byte of the data in order
		for (index = 0; index < len; ++index)
		{
			a = (a + data[index]) % MOD_ADLER;
			b = (b + a) % MOD_ADLER;
		}

		return (b << 16) | a;
	}
};

}