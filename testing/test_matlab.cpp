#include "meadow/math.h"
#include "meadow/matlab.h"

#include "meadow/span.h"

#include <gtest/gtest.h>

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

TEST(matlab, polyder)
{
    array<double, 4> cs{2, 3, 4, 5};
    ASSERT_EQ(matlab::polyder(span<const double>(cs.data(), 0)), std::vector<double>({0.0}));
    ASSERT_EQ(matlab::polyder(span<const double>(cs.data(), 1)), std::vector<double>({0.0}));
    ASSERT_EQ(matlab::polyder(span<const double>(cs.data(), 2)), std::vector<double>({2.0}));
    ASSERT_EQ(matlab::polyder(span<const double>(cs.data(), 3)), std::vector<double>({4.0, 3.0}));
    ASSERT_EQ(matlab::polyder(span<const double>(cs.data(), 4)), std::vector<double>({6.0, 6.0, 4.0}));
}

TEST(matlab, polyder_noalloc)
{
    array<double, 4> cs{2, 3, 4, 5};
    ASSERT_EQ(matlab::polyder_noalloc<12>(span<const double>(cs.data(), 0)), (std::inplace_vector<double, 12>({0.0})));
    ASSERT_EQ(matlab::polyder_noalloc<12>(span<const double>(cs.data(), 1)), (std::inplace_vector<double, 12>({0.0})));
    ASSERT_EQ(matlab::polyder_noalloc<12>(span<const double>(cs.data(), 2)), (std::inplace_vector<double, 12>({2.0})));
    ASSERT_EQ(
      matlab::polyder_noalloc<12>(span<const double>(cs.data(), 3)), (std::inplace_vector<double, 12>({4.0, 3.0}))
    );
    ASSERT_EQ(
      matlab::polyder_noalloc<12>(span<const double>(cs.data(), 4)), (std::inplace_vector<double, 12>({6.0, 6.0, 4.0}))
    );
}

TEST(matlab, roots2)
{
    {
        auto r = matlab::real_roots2<double>(array<double, 3>({3, 6, -105}));
        const auto e = std::inplace_vector<double, 2>({-7, 5});
        ASSERT_EQ(r, e);
    }
    {
        auto r = matlab::real_roots2<double>(array<double, 3>({3, -30, 75}));
        const auto e = std::inplace_vector<double, 2>({5});
        ASSERT_EQ(r, e);
    }
    {
        auto r = matlab::real_roots2<double>(array<double, 3>({3, 30, 75}));
        const auto e = std::inplace_vector<double, 2>({-5});
        ASSERT_EQ(r, e);
    }
    {
        auto r = matlab::real_roots2<double>(array<double, 3>({3, 5, 7}));
        ASSERT_TRUE(r.empty());
    }
    NOP;
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

TEST(matlab, polyfit)
{
    constexpr double eps = 1e-12;
    vector<double> xs = {-5, -3, 1, 2, 7, 11};
    vector<double> ys = {11, 13, 14, 7, 3, -2};
    {
        auto p = matlab::polyfit(mdspan_using_data_and_size(xs), mdspan_using_data_and_size(ys), 0);
        expect_near(p, vector<double>({7.666666666666668}), eps);
    }
    {
        auto p = matlab::polyfit(mdspan_using_data_and_size(xs), mdspan_using_data_and_size(ys), 1);
        expect_near(p, vector<double>({-0.921658986175115, 9.663594470046085}), eps);
    }
    {
        auto p = matlab::polyfit(mdspan_using_data_and_size(xs), mdspan_using_data_and_size(ys), 2);
        expect_near(p, vector<double>({-6.646772924152947e-02, -5.253642945037290e-01, 1.112024854000469e+01}), eps);
    }
    {
        auto p = matlab::polyfit(mdspan_using_data_and_size(xs), mdspan_using_data_and_size(ys), 3);
        expect_near(
          p,
          vector<double>(
            {1.071777767961007e-02, -1.590780807006455e-01, -8.295077324054219e-01, 1.227033362670373e+01}
          ),
          eps
        );
    }
    {
        auto p = matlab::polyfit(mdspan_using_data_and_size(xs), mdspan_using_data_and_size(ys), 4);
        expect_near(
          p,
          vector<double>(
            {-1.064021272966234e-03,
             2.394466621795351e-02,
             -1.485789244316916e-01,
             -1.148467745106906e+00,
             1.237102163786194e+01}
          ),
          eps
        );
    }
}

TEST(matlab, linspace)
{
    {
        const auto expected = vector<double>({2, 2.5, 3, 3.5, 4});
        ASSERT_EQ(matlab::linspace(2, 4, 5), expected);
    }
    {
        const auto expected = vector<double>({2, 0.5, -1, -2.5, -4});
        ASSERT_EQ(matlab::linspace(2, -4, 5), expected);
    }
}

static void assert_double_eq(const vector<double>& a, const vector<double>& b)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i) {
        ASSERT_DOUBLE_EQ(a[i], b[i]);
    }
}

TEST(matlab, logspace)
{
    {
        assert_double_eq(matlab::logspace(-1, 2, 4), vector<double>({0.1, 1, 10, 100}));
    }
    {
        const auto expected = vector<double>({10, 1, 0.1});
        ASSERT_EQ(matlab::logspace(1, -1, 3), expected);
    }
}

static void test_datenum(int year, int month, int day, double expected)
{
    EXPECT_EQ(matlab::datenum(year, month, day), expected);
}

TEST(matlab, datenum)
{
    test_datenum(1900, 1, 1, 693962);
    test_datenum(1999, 12, 31, 730485);
    test_datenum(2021, 12, 23, 738513);
}

TEST(matlab, cov)
{
    const auto xs = vector<double>({1, 3, 1, 4, 1, 5});
    const auto ys = vector<double>({2, 5, 8, 5, 8, 4});
    {
        // Default is the sample variance, w = 0.
        const auto c = matlab::cov(xs, ys);
        EXPECT_EQ(c, matlab::cov(xs, ys, 0));
        EXPECT_DOUBLE_EQ(c[0][0], 3.1);
        EXPECT_DOUBLE_EQ(c[0][1], -1.3999999999999997);
        EXPECT_DOUBLE_EQ(c[1][0], c[0][1]);
        EXPECT_DOUBLE_EQ(c[1][1], 5.466666666666667);
    }
    {
        const auto c = matlab::cov(xs, ys, 1);
        EXPECT_DOUBLE_EQ(c[0][0], 2.5833333333333335);
        EXPECT_DOUBLE_EQ(c[0][1], -1.1666666666666663);
        EXPECT_DOUBLE_EQ(c[1][0], c[0][1]);
        EXPECT_DOUBLE_EQ(c[1][1], 4.555555555555555);
    }
    {
        // cov(xs, xs) is the variance of xs in every element.
        const auto c = matlab::cov(xs, xs);
        EXPECT_DOUBLE_EQ(c[0][0], 3.1);
        EXPECT_DOUBLE_EQ(c[0][1], 3.1);
        EXPECT_DOUBLE_EQ(c[1][0], 3.1);
        EXPECT_DOUBLE_EQ(c[1][1], 3.1);
    }
    {
        // A single sample has zero population variance.
        const auto c = matlab::cov(span<const double>(xs).first(1), span<const double>(ys).first(1), 1);
        EXPECT_EQ(
          c,
          (std::array<std::array<double, 2>, 2>{
            {{0, 0}, {0, 0}}
        })
        );
    }
}

TEST(matlab, cov2)
{
    const auto xs = {1.0, 3.0, 1.0, 4.0, 1.0, 5.0};
    const auto ys = {2.0, 5.0, 8.0, 5.0, 8.0, 4.0};
    const auto a = matlab::cov(xs, ys);
    const auto a0 = matlab::cov(xs, ys, 0);
    const auto a1 = matlab::cov(xs, ys, 1);
    constexpr double eps = 1e-15;
    EXPECT_EQ(a, a0);
    EXPECT_NEAR(a0[0][0], 3.1, eps);
    EXPECT_NEAR(a0[0][1], -1.4, eps);
    EXPECT_EQ(a0[1][0], a0[1][0]);
    EXPECT_NEAR(a0[1][1], 5.466666666666667, eps);
    EXPECT_NEAR(a1[0][0], 2.583333333333333, eps);
    EXPECT_NEAR(a1[0][1], -1.166666666666666, eps);
    EXPECT_EQ(a1[1][0], a1[1][0]);
    EXPECT_NEAR(a1[1][1], 4.555555555555556, eps);
}

TEST(matlab, var)
{
    const auto xs = vector<double>({1, 3, 1, 4, 1, 5});
    // Default is the sample variance, w = 0.
    EXPECT_DOUBLE_EQ(matlab::var(xs), matlab::var(xs, 0));
    EXPECT_DOUBLE_EQ(matlab::var(xs), 3.1);
    EXPECT_DOUBLE_EQ(matlab::var(xs, 1), 2.5833333333333335);
    // Same as the diagonal of cov.
    EXPECT_DOUBLE_EQ(matlab::var(xs), matlab::cov(xs, xs)[0][0]);
    // A single sample has zero population variance.
    EXPECT_DOUBLE_EQ(matlab::var(span<const double>(xs).first(1), 1), 0.0);
    // A constant input has zero variance.
    EXPECT_DOUBLE_EQ(matlab::var(vector<double>({7, 7, 7})), 0.0);
}

TEST(matlab, std)
{
    const auto xs = vector<double>({1, 3, 1, 4, 1, 5});
    EXPECT_DOUBLE_EQ(matlab::std(xs), matlab::std(xs, 0));
    EXPECT_DOUBLE_EQ(matlab::std(xs), 1.760681686165901);
    EXPECT_DOUBLE_EQ(matlab::std(xs, 1), 1.6072751268321592);
    EXPECT_DOUBLE_EQ(matlab::std(xs), sqrt(matlab::var(xs)));
}
