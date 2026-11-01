#include "meadow/compat.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <string>

TEST(compat, format)
{
    EXPECT_EQ(compat::format("abc"), "abc");
    EXPECT_EQ(compat::format("{} {} {}", 1, "two", std::string("three")), "1 two three");
    EXPECT_EQ(compat::format("{:.2f}", 1.5), "1.50");
    EXPECT_EQ(compat::format("{:>4}|{:<4}|", 12, 'x'), "  12|x   |");
}

TEST(compat, print)
{
    testing::internal::CaptureStdout();
    compat::print("{}-", 1);
    compat::println("{}", 2.5);
    compat::print(stdout, "{}", "x");
    compat::println(stdout, "{}", 'y');
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1-2.5\nxy\n");
}
