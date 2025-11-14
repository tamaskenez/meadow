#include "meadow/matlab.h"

#include <gtest/gtest.h>

TEST(matlab, mag2db)
{
    EXPECT_DOUBLE_EQ(matlab::mag2db(0.0), -INFINITY);
    EXPECT_DOUBLE_EQ(matlab::mag2db(1.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::mag2db(10.0), 20.0);
}

TEST(matlab, pow2db)
{
    EXPECT_DOUBLE_EQ(matlab::pow2db(0.0), -INFINITY);
    EXPECT_DOUBLE_EQ(matlab::pow2db(1.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::pow2db(10.0), 10.0);
}

TEST(matlab, db2mag)
{
    EXPECT_DOUBLE_EQ(matlab::db2mag(0.0), 1.0);
    EXPECT_DOUBLE_EQ(matlab::db2mag(20.0), 10.0);
}

TEST(matlab, db2pow)
{
    EXPECT_DOUBLE_EQ(matlab::db2pow(0.0), 1.0);
    EXPECT_DOUBLE_EQ(matlab::db2pow(10.0), 10.0);
}

TEST(matlab, mean)
{
    vector<double> vs({1.0, 2.0, 3.0});
    EXPECT_DOUBLE_EQ(matlab::mean(span<double>(vs.data(), 1)), 1.0);
    EXPECT_DOUBLE_EQ(matlab::mean(span<double>(vs.data(), 2)), 1.5);
    EXPECT_DOUBLE_EQ(matlab::mean(span<const double>(vs.data(), 3)), 2.0);
}

TEST(matlab, mean_int)
{
    vector<int> vs({1, 2, 3});
    EXPECT_DOUBLE_EQ(matlab::mean<double>(span<int>(vs.data(), 1)), 1.0);
    EXPECT_DOUBLE_EQ(matlab::mean<double>(span<int>(vs.data(), 2)), 1.5);
    EXPECT_DOUBLE_EQ(matlab::mean<double>(span<const int>(vs.data(), 3)), 2.0);
}

TEST(matlab, interp1)
{
    {
        vector<double> x = {7.0};
        vector<double> y = {8.0};
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 6.9), 8.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 7.0), 8.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 7.1), 8.0);
    }
    {
        vector<double> x = {7.0, 9.0};
        vector<double> y = {8.0, 12.0};
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 6.9), 8.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 7.0), 8.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 7.5), 9.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 8.0), 10.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 8.5), 11.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 9.0), 12.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 9.1), 12.0);
    }
    {
        vector<double> x = {7.0, 9.0, 10.0};
        vector<double> y = {8.0, 12.0, 20.0};
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 6.9), 8.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 7.0), 8.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 7.5), 9.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 8.0), 10.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 8.5), 11.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 9.0), 12.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 9.5), 16.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 10.0), 20.0);
        EXPECT_DOUBLE_EQ(matlab::interp1(x, y, 10.1), 20.0);
    }
}

TEST(matlab, deg2rad)
{
    EXPECT_DOUBLE_EQ(matlab::deg2rad(0.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::deg2rad(90.0), num::pi / 2);
    EXPECT_DOUBLE_EQ(matlab::deg2rad(-90.0), -num::pi / 2);
    EXPECT_DOUBLE_EQ(matlab::deg2rad(0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::deg2rad(90), num::pi / 2);
    EXPECT_DOUBLE_EQ(matlab::deg2rad(-90), -num::pi / 2);
}

TEST(matlab, rad2deg)
{
    EXPECT_DOUBLE_EQ(matlab::rad2deg(0.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::rad2deg(num::pi / 2), 90);
    EXPECT_DOUBLE_EQ(matlab::rad2deg(-num::pi / 2), -90);
}

namespace
{
template<class Fn>
vector<double> make_win(size_t N, Fn&& fn)
{
    EXPECT_EQ(fn(-1), 0.0);
    EXPECT_EQ(fn(iicast<int>(N)), 0.0);
    vector<double> xs(N);
    for (size_t i : vi::iota(0u, N)) {
        xs[i] = fn(iicast<int>(i));
    }
    return xs;
}
} // namespace

TEST(matlab, rectwin_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::rectwin_fn(n, 7);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::rectwin_fn(n, 8);
    });
    EXPECT_EQ(r7, vector<double>({1, 1, 1, 1, 1, 1, 1}));
    EXPECT_EQ(r8, vector<double>({1, 1, 1, 1, 1, 1, 1, 1}));
}

namespace
{
void expect_near(const vector<double>& x, const vector<double>& y, double e)
{
    ASSERT_EQ(x.size(), y.size());
    for (size_t i : vi::iota(0u, x.size())) {
        EXPECT_NEAR(x[i], y[i], e);
    }
}
} // namespace

TEST(matlab, blackman_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::blackman_fn(n, 7);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::blackman_fn(n, 8);
    });

    expect_near(
      r7,
      vector<double>(
        {-1.387778780781446e-17,
         1.299999999999999e-01,
         6.299999999999999e-01,
         9.999999999999999e-01,
         6.300000000000002e-01,
         1.299999999999999e-01,
         -1.387778780781446e-17}
      ),
      DBL_EPSILON
    );
    expect_near(
      r8,
      vector<double>(
        {-1.387778780781446e-17,
         9.045342435412804e-02,
         4.591829575459636e-01,
         9.203636180999081e-01,
         9.203636180999083e-01,
         4.591829575459638e-01,
         9.045342435412812e-02,
         -1.387778780781446e-17}
      ),
      DBL_EPSILON
    );
}

TEST(matlab, gausswin_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::gausswin_fn(n, 7, 3.0);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::gausswin_fn(n, 8, 4.0);
    });

    expect_near(
      r7,
      vector<double>(
        {1.110899653824231e-02,
         1.353352832366127e-01,
         6.065306597126334e-01,
         1.000000000000000e+00,
         6.065306597126334e-01,
         1.353352832366127e-01,
         1.110899653824231e-02}
      ),
      DBL_EPSILON
    );
    expect_near(
      r8,
      vector<double>(
        {3.354626279025119e-04,
         1.687988414878993e-02,
         2.300662989938091e-01,
         8.493658165683124e-01,
         8.493658165683124e-01,
         2.300662989938091e-01,
         1.687988414878993e-02,
         3.354626279025119e-04}
      ),
      DBL_EPSILON
    );
}

#if MEADOW_HAS_CYL_BESSEL_I == 1 || MEADOW_HAS_BOOST == 1
TEST(matlab, kaiser_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::kaiser_fn(n, 7, 1.23);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::kaiser_fn(n, 8, 1.78);
    });

    expect_near(
      r7,
      vector<double>(
        {0.706450386180156,
         0.862875634246974,
         0.964683201523233,
         1.000000000000000,
         0.964683201523233,
         0.862875634246974,
         0.706450386180156}
      ),
      1e-15
    );
    expect_near(
      r8,
      vector<double>(
        {0.509303391023423,
         0.726907890745535,
         0.895844495440142,
         0.988089123045320,
         0.988089123045320,
         0.895844495440142,
         0.726907890745535,
         0.509303391023423}
      ),
      1e-15
    );
}
#endif

TEST(matlab, hann_poisson_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::hann_poisson_fn(n, 7, 2.1);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::hann_poisson_fn(n, 8, 2.9);
    });

    expect_near(
      r7,
      vector<double>(
        {0, 0.061649240985402, 0.372438977843557, 1.000000000000000, 0.372438977843557, 0.061649240985402, 0}
      ),
      1e-15
    );
    expect_near(
      r8,
      vector<double>(
        {0,
         0.023721205183369,
         0.176384455982313,
         0.628091634613610,
         0.628091634613610,
         0.176384455982313,
         0.023721205183369,
         0}
      ),
      1e-15
    );
}

TEST(matlab, nextpow2_int8_t)
{
    const int8_t expected[] = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                               7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                               7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                               6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4,
                               4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 1, 0, 0, 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
                               5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                               6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                               7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                               7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    static_assert(sizeof(expected) == 256);
    for (int8_t x : vi::iota(int8_t(-128), int8_t(127))) {
        EXPECT_EQ(matlab::nextpow2(x), expected[x + 128]);
    }
}

TEST(matlab, nextpow2_uint8_t)
{
    const uint8_t expected[] = {0, 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                                6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                                8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
    static_assert(sizeof(expected) == 256);
    for (uint8_t x : vi::iota(uint8_t(0), uint8_t(255))) {
        EXPECT_EQ(matlab::nextpow2(x), expected[x]);
    }
}

TEST(matlab, nextpow2_int)
{
    EXPECT_EQ(matlab::nextpow2(12345), 14);
    EXPECT_EQ(matlab::nextpow2(65535), 16);
    EXPECT_EQ(matlab::nextpow2(65536), 16);
    EXPECT_EQ(matlab::nextpow2(65537), 17);
}

TEST(matlab, polyfit1)
{
    vector<double> xs = {1, -2, 5, 6, 9, 10};
    vector<double> ys = {-3, 5, 4, 8, 10, 11};
    auto r = matlab::polyfit1<double>(xs, ys);
    EXPECT_NEAR(r[0], 0.803432137285491, 1e-12);
    EXPECT_NEAR(r[1], 1.950078003120126, 1e-12);

    xs = {1.0, 2.0, 3.0};
    ys = {3.0, 5.0, 7.0};
    r = matlab::polyfit1<double>(xs, ys);
    EXPECT_DOUBLE_EQ(r[0], 2.0);
    EXPECT_DOUBLE_EQ(r[1], 1.0);
}

TEST(matlab, polyval)
{
    array<double, 4> cs{2, 3, 4, 5};
    constexpr double x = 7;
    ASSERT_EQ(matlab::polyval(span<const double>(cs.data(), 0), x), 0.0);
    ASSERT_EQ(matlab::polyval(span<const double>(cs.data(), 1), x), 2.0);
    ASSERT_EQ(matlab::polyval(span<const double>(cs.data(), 2), x), 17.0);
    ASSERT_EQ(matlab::polyval(span<const double>(cs.data(), 3), x), 123.0);
    ASSERT_EQ(matlab::polyval(span<const double>(cs.data(), 4), x), 866.0);
}
