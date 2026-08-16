#include "meadow/math.h"

#include <gtest/gtest.h>

namespace
{
void expectDoubleEq(pair<double, double> a, pair<double, double> b)
{
    EXPECT_NEAR(a.first, b.first, 1e-10);
    EXPECT_NEAR(a.second, b.second, 1e-10);
#ifndef _MSC_VER
    EXPECT_DOUBLE_EQ(a.first, b.first);
    EXPECT_DOUBLE_EQ(a.second, b.second);
#endif
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

static void test_regspace(double begin, double end, double step, const vector<double>& expected)
{
#ifndef NDEBUG
    if (expected.empty()) {
        return;
    }
#endif
    auto r = regspace(begin, step, end);

    ASSERT_EQ(r.size(), expected.size());
    for (size_t i = 0; i < r.size(); ++i) {
        ASSERT_DOUBLE_EQ(r[i], expected[i]);
    }
}

TEST(math, regspace)
{
    test_regspace(1.0, 1.0, -0.3, {1.0});
    test_regspace(1.0, 1.0, 0.0, {});
    test_regspace(1.0, 1.0, 0.3, {1.0});
    test_regspace(1.0, 2.0, -0.3, {});
    test_regspace(1.0, 2.0, 0.0, {});
    test_regspace(1.0, 2.0, 0.3, {1.0, 1.3, 1.6, 1.9});
    test_regspace(2.0, 1.0, -0.3, {2.0, 1.7, 1.4, 1.1});
    test_regspace(2.0, 1.0, 0.0, {});
    test_regspace(2.0, 1.0, 0.3, {});
    test_regspace(1.0, 1.0, -1.0, {1.0});
    test_regspace(1.0, 1.0, 1.0, {1.0});
    test_regspace(1.0, 2.0, -1.0, {});
    test_regspace(1.0, 2.0, 1.0, {1.0, 2.0});
    test_regspace(2.0, 1.0, -1.0, {2.0, 1.0});
    test_regspace(2.0, 1.0, 1.0, {});
    test_regspace(1.0, 1.0, -2.0, {1.0});
    test_regspace(1.0, 1.0, 2.0, {1.0});
    test_regspace(1.0, 2.0, -2.0, {});
    test_regspace(1.0, 2.0, 2.0, {1.0});
    test_regspace(2.0, 1.0, -2.0, {2.0});
    test_regspace(2.0, 1.0, 2.0, {});
    test_regspace(0.0, 1.0, 0.1, {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0});
    test_regspace(0.0, 1.0, 1.0 / 3.0, {0.0, 1.0 / 3.0, 2.0 / 3.0, 1.0});
    test_regspace(0.0, -1.0, -1.0 / 3.0, {0.0, -1.0 / 3.0, -2.0 / 3.0, -1.0});
    test_regspace(0.0, 2.0, 2.0 / 3.0, {0.0, 2.0 / 3.0, 4.0 / 3.0, 2.0});
}

static void test_floor_frac(double x, double i, double f)
{
    auto [integral, fractional] = floor_frac(x);
    EXPECT_DOUBLE_EQ(i, integral);
    EXPECT_DOUBLE_EQ(f, fractional);
}

TEST(math, floor_frac)
{
    test_floor_frac(1.5, 1, 0.5);
    test_floor_frac(1.0, 1, 0);
    test_floor_frac(0.9, 0, 0.9);
    test_floor_frac(0.1, 0, 0.1);
    test_floor_frac(0.0, 0, 0);
    test_floor_frac(-0.0, 0, 0);
    test_floor_frac(-0.1, -1, 0.9);
    test_floor_frac(-0.9, -1, 0.1);
    test_floor_frac(-1.0, -1, 0);
    test_floor_frac(-1.5, -2, 0.5);
}

static void test_ifloor_frac(double x, int i, double f)
{
    auto [integral, fractional] = ifloor_frac<int>(x);
    EXPECT_EQ(i, integral);
    EXPECT_DOUBLE_EQ(f, fractional);
}

TEST(math, ifloor_frac)
{
    test_ifloor_frac(1.5, 1, 0.5);
    test_ifloor_frac(1.0, 1, 0);
    test_ifloor_frac(0.9, 0, 0.9);
    test_ifloor_frac(0.1, 0, 0.1);
    test_ifloor_frac(0.0, 0, 0);
    test_ifloor_frac(-0.0, 0, 0);
    test_ifloor_frac(-0.1, -1, 0.9);
    test_ifloor_frac(-0.9, -1, 0.1);
    test_ifloor_frac(-1.0, -1, 0);
    test_ifloor_frac(-1.5, -2, 0.5);
}

namespace
{
RunningStat makeRunningStat(std::initializer_list<double> samples)
{
    RunningStat rs;
    for (auto s : samples) {
        rs(s);
    }
    return rs;
}
} // namespace

// The queries assert on an insufficient number of samples, they return NaN only when asserts are disabled.
TEST(math, RunningStat_empty)
{
    const RunningStat rs;
    EXPECT_EQ(rs.count(), 0u);
#ifdef NDEBUG
    EXPECT_TRUE(std::isnan(rs.mean()));
    EXPECT_TRUE(std::isnan(rs.var()));
    EXPECT_TRUE(std::isnan(rs.var(VarianceNorm::population)));
    EXPECT_TRUE(std::isnan(rs.stddev()));
    EXPECT_TRUE(std::isnan(rs.min()));
    EXPECT_TRUE(std::isnan(rs.max()));
    EXPECT_TRUE(std::isnan(rs.range()));
#endif
}

TEST(math, RunningStat_single_sample)
{
    const auto rs = makeRunningStat({3.5});
    EXPECT_EQ(rs.count(), 1u);
    EXPECT_DOUBLE_EQ(rs.mean(), 3.5);
#ifdef NDEBUG
    EXPECT_TRUE(std::isnan(rs.var()));
#endif
    EXPECT_DOUBLE_EQ(rs.var(VarianceNorm::population), 0.0);
    EXPECT_DOUBLE_EQ(rs.stddev(VarianceNorm::population), 0.0);
    EXPECT_DOUBLE_EQ(rs.min(), 3.5);
    EXPECT_DOUBLE_EQ(rs.max(), 3.5);
    EXPECT_DOUBLE_EQ(rs.range(), 0.0);
}

TEST(math, RunningStat_moments)
{
    const auto rs = makeRunningStat({2, 4, 4, 4, 5, 5, 7, 9});
    EXPECT_EQ(rs.count(), 8u);
    EXPECT_DOUBLE_EQ(rs.mean(), 5.0);
    EXPECT_DOUBLE_EQ(rs.var(VarianceNorm::population), 4.0);
    EXPECT_DOUBLE_EQ(rs.stddev(VarianceNorm::population), 2.0);
    EXPECT_DOUBLE_EQ(rs.var(), 32.0 / 7);
    EXPECT_DOUBLE_EQ(rs.stddev(), sqrt(32.0 / 7));
    EXPECT_DOUBLE_EQ(rs.min(), 2.0);
    EXPECT_DOUBLE_EQ(rs.max(), 9.0);
    EXPECT_DOUBLE_EQ(rs.range(), 7.0);
}

TEST(math, RunningStat_negative_samples)
{
    const auto rs = makeRunningStat({-1.0, -3.0, 2.0});
    EXPECT_DOUBLE_EQ(rs.mean(), -2.0 / 3);
    EXPECT_DOUBLE_EQ(rs.var(), 19.0 / 3);
    EXPECT_DOUBLE_EQ(rs.min(), -3.0);
    EXPECT_DOUBLE_EQ(rs.max(), 2.0);
    EXPECT_DOUBLE_EQ(rs.range(), 5.0);
}

// The variance of samples with a large offset is where the naive sum-of-squares formula loses all precision.
TEST(math, RunningStat_large_offset)
{
    const double offset = 1e9;
    const auto rs = makeRunningStat({offset + 4, offset + 7, offset + 13, offset + 16});
    EXPECT_DOUBLE_EQ(rs.mean(), offset + 10);
    EXPECT_DOUBLE_EQ(rs.var(), 30.0);
    EXPECT_DOUBLE_EQ(rs.var(VarianceNorm::population), 22.5);
}

TEST(math, RunningStat_reset)
{
    auto rs = makeRunningStat({1.0, 2.0});
    rs.reset();
    EXPECT_EQ(rs.count(), 0u);

    rs(8.0);
    EXPECT_EQ(rs.count(), 1u);
    EXPECT_DOUBLE_EQ(rs.mean(), 8.0);
    EXPECT_DOUBLE_EQ(rs.max(), 8.0);
}
