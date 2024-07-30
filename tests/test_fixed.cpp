#include <catch2/catch_test_macros.hpp>
#include "maths/fixed.h"

TEST_CASE("Fixed", "Construct")
{
    constexpr std::int32_t i = 32;
    constexpr auto f = splash::Fixed{ i };
    REQUIRE(i == static_cast<std::int32_t>(f));

    constexpr auto f2 = splash::Fixed8{ 1 };
    constexpr auto f3 = splash::Fixed8{ -1 };
    REQUIRE(1 == static_cast<std::int8_t>(f2));
    REQUIRE(-1 == static_cast<std::int8_t>(f3));

    //Fixed8 is too small to contain 2
    constexpr auto f4 = splash::Fixed8 { 2u };
    REQUIRE(2 != static_cast<std::int8_t>(f4));
}

