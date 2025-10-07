#include "meadow/math.h"

#include <gtest/gtest.h>

namespace
{
void expectDoubleEq(pair<double, double> a, pair<double, double> b)
{
    EXPECT_DOUBLE_EQ(a.first, b.first);
    EXPECT_DOUBLE_EQ(a.second, b.second);
}

#if MEADOW_HAS_EIGEN == 1
pair<double, double> callExtremumOfParabola(initializer_list<double> xs, initializer_list<double> ys)
{
    vector<double> vxs(xs);
    vector<double> vys(ys);
    return extremumOfParabola<double, double, double>(span(vxs.data(), vxs.size()), span(vys.data(), vys.size()));
}
#endif

} // namespace

TEST(math, extremumOfParabola)
{
    expectDoubleEq(extremumOfParabola(-1, 0, -1), pair(0.0, 0.0));
    expectDoubleEq(extremumOfParabola(1, 0, 1), pair(0.0, 0.0));
    expectDoubleEq(extremumOfParabola(-1, 1, -1), pair(0.0, 1.0));
    expectDoubleEq(extremumOfParabola(1, -1, 1), pair(0.0, -1.0));
    expectDoubleEq(extremumOfParabola(-1, 1, 1), pair(0.5, 1.25));
    expectDoubleEq(extremumOfParabola(-5, 2, 1), pair(0.375, 2.5625));
}

#if MEADOW_HAS_EIGEN == 1
TEST(math, extremumOfParabola2)
{
    expectDoubleEq(callExtremumOfParabola({-1, 0, 1}, {-1, 0, -1}), pair(0.0, 0.0));
    expectDoubleEq(callExtremumOfParabola({-1, 0, 1}, {1, 0, 1}), pair(0.0, 0.0));
    expectDoubleEq(callExtremumOfParabola({-1, 0, 1}, {-1, 1, -1}), pair(0.0, 1.0));
    expectDoubleEq(callExtremumOfParabola({-1, 0, 1}, {1, -1, 1}), pair(0.0, -1.0));
    expectDoubleEq(callExtremumOfParabola({-1, 0, 1}, {-1, 1, 1}), pair(0.5, 1.25));
    expectDoubleEq(callExtremumOfParabola({-1, 0, 1}, {-5, 2, 1}), pair(0.375, 2.5625));

    expectDoubleEq(callExtremumOfParabola({5, 7, 11}, {2, 5, 3}), pair(8.250000000000000, 5.520833333333333));
    expectDoubleEq(callExtremumOfParabola({-5, -7, -11}, {-2, -5, -3}), pair(-8.250000000000000, -5.520833333333333));
    expectDoubleEq(
      callExtremumOfParabola({-40, 121, 542}, {1000, 1234, -321}), pair(122.67289011061459, 1234.024749489852184)
    );

    expectDoubleEq(
      callExtremumOfParabola(
        {14, 15, 1, 3, -2, 9, 15}, {382.2619, 465.5079, -30.9207, 78.0318, -0.4457, 67.0521, 505.4352}
      ),
      pair(1.6925948689722556, -14.222691885565183)
    );
}
#endif

TEST(math, is_even)
{
    EXPECT_FALSE(is_even(-3));
    EXPECT_TRUE(is_even(-2));
    EXPECT_FALSE(is_even(-1));
    EXPECT_TRUE(is_even(0));
    EXPECT_FALSE(is_even(1));
    EXPECT_TRUE(is_even(2));
    EXPECT_FALSE(is_even(3));
}

TEST(math, is_odd)
{
    EXPECT_TRUE(is_odd(-3));
    EXPECT_FALSE(is_odd(-2));
    EXPECT_TRUE(is_odd(-1));
    EXPECT_FALSE(is_odd(0));
    EXPECT_TRUE(is_odd(1));
    EXPECT_FALSE(is_odd(2));
    EXPECT_TRUE(is_odd(3));
}

template<class T>
void test_sgn_floating()
{
    using std::numeric_limits;
    static_assert(std::is_same_v<std::decay_t<decltype(sgn(T(0)))>, T>);
    EXPECT_TRUE(std::isnan(sgn(numeric_limits<T>::quiet_NaN())));
    EXPECT_TRUE(std::isnan(sgn(numeric_limits<T>::signaling_NaN())));
    EXPECT_EQ(sgn(-std::numeric_limits<T>::infinity()), T(-1));
    EXPECT_EQ(sgn(T(-10)), T(-1));
    EXPECT_EQ(sgn(-numeric_limits<T>::denorm_min()), T(-1));
    EXPECT_EQ(sgn(T(0)), T(0));
    EXPECT_EQ(sgn(numeric_limits<T>::denorm_min()), T(1));
    EXPECT_EQ(sgn(T(10)), T(1));
    EXPECT_EQ(sgn(numeric_limits<T>::infinity()), T(1));
}

TEST(math, sgn_floating)
{
    test_sgn_floating<float>();
    test_sgn_floating<double>();
}

TEST(math, sgn_unsigned_integral)
{
    uint8_t x = 0;
    do {
        const auto sgn_x = sgn(x);
        static_assert(std::is_same_v<std::decay_t<decltype(sgn_x)>, uint8_t>);
        if (x == 0) {
            EXPECT_EQ(sgn_x, uint8_t(0));
        } else {
            EXPECT_EQ(sgn_x, uint8_t(1));
        }
    } while (x++ != 255);
}

TEST(math, sgn_signed_integral)
{
    int8_t x = -128;
    do {
        const auto sgn_x = sgn(x);
        static_assert(std::is_same_v<std::decay_t<decltype(sgn_x)>, int8_t>);
        if (x == 0) {
            EXPECT_EQ(sgn_x, int8_t(0));
        } else {
            EXPECT_EQ(sgn_x, int8_t(x < 0 ? -1 : 1));
        }
    } while (x++ != 127);
}

TEST(math, modulo_floating_point)
{
    constexpr double M = 2.5;
    for (int i : vi::iota(-10, 10)) {
        EXPECT_EQ(modulo(0.0 + i * M, M), 0.0);
        EXPECT_EQ(modulo(0.5 + i * M, M), 0.5);
        EXPECT_EQ(modulo(2.0 + i * M, M), 2.0);
    }
}

TEST(math, module_integral)
{
    constexpr int M = 25;
    for (int i : vi::iota(-10, 10)) {
        EXPECT_EQ(modulo(0 + i * M, M), 0);
        EXPECT_EQ(modulo(5 + i * M, M), 5);
        EXPECT_EQ(modulo(20 + i * M, M), 20);
    }
}
