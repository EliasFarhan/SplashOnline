//
// Created by unite on 15.07.2024.
//

#ifndef SPLASHONLINE_ROLLBACK_SYSTEM_H
#define SPLASHONLINE_ROLLBACK_SYSTEM_H

#include <container/vector.h>

#include <cstdint>
#include <initializer_list>
#include <numeric>

namespace splash
{

template<int argCount>
struct Checksum
{
	Checksum(std::initializer_list<std::uint32_t> list )
	{
		for(std::size_t i = 0; i < list.size(); i++)
		{
			data_.push_back(data(list)[i]);
		}
	}
	bool operator==(const Checksum& other) const
	{
		return data_ == other;
	}
	bool operator!=(const Checksum& other) const
	{
		return data_ != other.data_;
	}
	std::uint32_t operator[](std::size_t index) const
	{
		return data_[index];
	}

	[[nodiscard]] explicit operator std::uint32_t() const
	{
		return std::accumulate(data_.cbegin(), data_.cend(), 0);
	}

	template<int otherArgCount>
	void push_back(const Checksum<otherArgCount>& otherChecksum )
	{
		for(int i = 0; i < otherArgCount; i++)
		{
			data_.push_back(otherChecksum[i]);
		}
	}
private:
	neko::SmallVector<std::uint32_t, argCount> data_{};
};

	template<typename T, int argCount>
    class RollbackInterface
    {
    public:
		virtual ~RollbackInterface() = default;
        [[nodiscard]] virtual Checksum<argCount> CalculateChecksum() const = 0;
        virtual void RollbackFrom(const T& system) = 0;
    };

}


#endif //SPLASHONLINE_ROLLBACK_SYSTEM_H
