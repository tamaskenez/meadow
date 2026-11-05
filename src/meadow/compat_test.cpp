#include "meadow/compat.h"

#include <gtest/gtest.h>

#include <chrono>
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
    compat::println();
    compat::println(stdout);
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1-2.5\nxy\n\n\n");
}

namespace test_compat_adl
{
using compat::format;
using compat::println;

// With std-typed arguments ADL would also find std::format (<chrono> includes it), which must not be selected when
// std::format is not available.
TEST(compat, unqualified_call_with_std_arguments)
{
    EXPECT_EQ(format("{} {}", std::string("a"), 1.5), "a 1.5");
    testing::internal::CaptureStdout();
    println("{}", std::string("b"));
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "b\n");
}
} // namespace test_compat_adl
