#include "meadow/cppext.h"

#include <gtest/gtest.h>

TEST(cppext, ifloor)
{
    static_assert(std::is_same_v<decltype(ifloor<uint16_t>(0.0)), uint16_t>);
    EXPECT_EQ(ifloor<int>(-1.1), -2);
    EXPECT_EQ(ifloor<int>(-1.0), -1);
    EXPECT_EQ(ifloor<int>(-0.9), -1);
    EXPECT_EQ(ifloor<int>(-0.5), -1);
    EXPECT_EQ(ifloor<int>(-0.1), -1);
    EXPECT_EQ(ifloor<int>(0.0), 0);
    EXPECT_EQ(ifloor<int>(0.1), 0);
    EXPECT_EQ(ifloor<int>(0.5), 0);
    EXPECT_EQ(ifloor<int>(0.9), 0);
    EXPECT_EQ(ifloor<int>(1.0), 1);
    EXPECT_EQ(ifloor<int>(1.1), 1);
}

TEST(cppext, iround)
{
    static_assert(std::is_same_v<decltype(iround<uint16_t>(0.0)), uint16_t>);
    EXPECT_EQ(iround<int>(-1.1), -1);
    EXPECT_EQ(iround<int>(-1.0), -1);
    EXPECT_EQ(iround<int>(-0.9), -1);
    EXPECT_EQ(iround<int>(-0.5), -1);
    EXPECT_EQ(iround<int>(-0.1), 0);
    EXPECT_EQ(iround<int>(0.0), 0);
    EXPECT_EQ(iround<int>(0.1), 0);
    EXPECT_EQ(iround<int>(0.5), 1);
    EXPECT_EQ(iround<int>(0.9), 1);
    EXPECT_EQ(iround<int>(1.0), 1);
    EXPECT_EQ(iround<int>(1.1), 1);
}

TEST(cppext, iceil)
{
    static_assert(std::is_same_v<decltype(iceil<uint16_t>(0.0)), uint16_t>);
    EXPECT_EQ(iceil<int>(-1.1), -1);
    EXPECT_EQ(iceil<int>(-1.0), -1);
    EXPECT_EQ(iceil<int>(-0.9), 0);
    EXPECT_EQ(iceil<int>(-0.5), 0);
    EXPECT_EQ(iceil<int>(-0.1), 0);
    EXPECT_EQ(iceil<int>(0.0), 0);
    EXPECT_EQ(iceil<int>(0.1), 1);
    EXPECT_EQ(iceil<int>(0.5), 1);
    EXPECT_EQ(iceil<int>(0.9), 1);
    EXPECT_EQ(iceil<int>(1.0), 1);
    EXPECT_EQ(iceil<int>(1.1), 2);
}

TEST(cppext, iicast)
{
    static_assert(std::is_same_v<decltype(iicast<uint16_t>(int8_t(0))), uint16_t>);
    EXPECT_EQ(iicast<uint8_t>(255), uint8_t(255));
}
