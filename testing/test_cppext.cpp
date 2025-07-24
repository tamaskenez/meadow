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

namespace
{
void expect_same_span(span<int> a, span<int> b)
{
    EXPECT_EQ(a.size(), b.size());
    if (!a.empty()) {
        EXPECT_EQ(a.data(), b.data());
    }
}
} // namespace

TEST(cppext, remove_prefix)
{
    vector<int> xs = {12, 34, 54};
    const auto s0 = span(BEGIN_END(xs));
    for (size_t i = 0; i <= 3; ++i) {
        auto s1 = s0;
        remove_prefix(s1, i);
        expect_same_span(s0.subspan(i, 3 - i), s1);
    }
}

TEST(cppext, remove_postfix)
{
    vector<int> xs = {12, 34, 54};
    const auto s0 = span(BEGIN_END(xs));
    for (size_t i = 0; i <= 3; ++i) {
        auto s1 = s0;
        remove_postfix(s1, i);
        expect_same_span(s0.subspan(0, 3 - i), s1);
    }
}
