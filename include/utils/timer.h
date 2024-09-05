//
// Created by unite on 06.07.2024.
//

#ifndef SPLASHONLINE_TIMER_H
#define SPLASHONLINE_TIMER_H

#include <math/const.h>

template<typename T=neko::Scalar>
class Timer
{
public:
    constexpr explicit Timer(T time, T period): time_(time), period_(period){}

    constexpr void Update(T deltaTime) noexcept
    {
        if (time_ >= T{0.0f})
            time_ -= deltaTime;

    }
    [[nodiscard]] constexpr bool Over() const noexcept
    {
        return time_ <= T{0.0f};
    }
    constexpr void SetPeriod(T newPeriod) noexcept
    {
        if (newPeriod < time_)
        {
            time_ = T{-1.0f};
        }
        period_ = newPeriod;
    }
	constexpr void SetTime(T newTime) noexcept
	{
		time_ = newTime;
	}
    constexpr void Reset() noexcept
    {
        time_ = period_;
    }
    constexpr void Stop() noexcept
    {
        time_ = T{-1.0f};
    }
    [[nodiscard]] constexpr T CurrentRatio() const noexcept
    {
        T current = (period_ - time_) / period_;
        if (current < T{0.0f})
            current = T{0.0f};
        if (current > T{1.0f})
            current = T{1.0f};
        return current;
    }
    [[nodiscard]] constexpr T CurrentTime() const noexcept
    {
        return period_ - time_;
    }
    [[nodiscard]] constexpr T RemainingTime() const noexcept
    {
        return time_;
    }
	[[nodiscard]] constexpr T GetPeriod() const noexcept
	{
		return period_;
	}
private:
    T time_{};
    T period_{};
};

#endif //SPLASHONLINE_TIMER_H
