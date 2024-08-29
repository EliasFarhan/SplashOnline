//
// Created by unite on 15.07.2024.
//

#ifndef SPLASHONLINE_ROLLBACK_SYSTEM_H
#define SPLASHONLINE_ROLLBACK_SYSTEM_H

#include <cstdint>
#include <initializer_list>

namespace splash
{

template<int argCount>
struct Checksum
{
	Checksum(std::initializer_list<std::uint32_t> list ){
		for(std::size_t i = 0; i < list.size(); i++)
		{
			data_[i] = data(list)[i];
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
private:
	std::array<std::uint32_t, argCount> data_{};
};

	template<typename T, int argCount>
    class RollbackInterface
    {
    public:
        [[nodiscard]] virtual Checksum<argCount> CalculateChecksum() const = 0;
        virtual void RollbackFrom(const T& system) = 0;
    };

}


#endif //SPLASHONLINE_ROLLBACK_SYSTEM_H
