//
// Created by unite on 06.07.2024.
//

#ifndef SPLASHONLINE_TIMER_H
#define SPLASHONLINE_TIMER_H

template<typename T=float>
class Timer
{
public:
    constexpr explicit Timer(T time, T period): time_(time), period_(period){}

    constexpr void Update(float deltaTime) noexcept
    {
        if (time_ >= 0)
            time_ -= deltaTime;

    }
    [[nodiscard]] constexpr bool Over() const noexcept
    {
        return time_ <= 0;
    }
    constexpr void SetPeriod(float newPeriod) noexcept
    {
        if (newPeriod < time_)
        {
            time_ = -1.0f;
        }
        period_ = newPeriod;
    }
    constexpr void Reset() noexcept
    {
        time_ = period_;
    }
    constexpr void Stop() noexcept
    {
        time_ = -1;
    }
    [[nodiscard]] constexpr T CurrentRatio() const noexcept
    {
        T current = (period_ - time_) / period_;
        if (current < 0)
            current = 0;
        if (current > 1)
            current = 1;
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
private:
    T time_{};
    T period_{};
};

#endif //SPLASHONLINE_TIMER_H
