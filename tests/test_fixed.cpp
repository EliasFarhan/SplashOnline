#include <catch2/catch_test_macros.hpp>
#include "maths/fixed.h"

TEST_CASE("Fixed", "Construct")
{
    constexpr std::int32_t i = 32;
    constexpr auto f = splash::Fixed{ i };
    REQUIRE(i == static_cast<std::int32_t>(f));
}