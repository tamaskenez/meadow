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

template<class X, class Y>
void test_signed_subtract_2()
{
    const auto x_min = std::numeric_limits<X>::min();
    const auto x_max = std::numeric_limits<X>::max();
    const auto y_min = std::numeric_limits<Y>::min();
    const auto y_max = std::numeric_limits<Y>::max();
    ASSERT_EQ(signed_subtract(x_min, y_max), iicast<int64_t>(x_min) - iicast<int64_t>(y_max));
    ASSERT_EQ(signed_subtract(x_max, y_min), iicast<int64_t>(x_max) - iicast<int64_t>(y_min));
}

template<class X>
void test_signed_subtract_1()
{
    test_signed_subtract_2<X, uint8_t>();
    test_signed_subtract_2<X, uint16_t>();
    test_signed_subtract_2<X, uint32_t>();
    test_signed_subtract_2<X, int8_t>();
    test_signed_subtract_2<X, int16_t>();
    test_signed_subtract_2<X, int32_t>();
}

TEST(cppext, signed_subtract)
{
    test_signed_subtract_1<uint8_t>();
    test_signed_subtract_1<uint16_t>();
    test_signed_subtract_1<uint32_t>();
    test_signed_subtract_1<int8_t>();
    test_signed_subtract_1<int16_t>();
    test_signed_subtract_1<int32_t>();

    ASSERT_EQ(signed_subtract(uint64_t(3), uint64_t(2)), 1);
    ASSERT_EQ(signed_subtract(uint64_t(2), uint64_t(3)), -1);
}
