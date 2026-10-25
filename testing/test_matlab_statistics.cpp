#include "meadow/matlab_statistics.h"

#include "meadow/span.h"

#include <gtest/gtest.h>

TEST(matlab_statistics, corr)
{
    const auto xs = vector<double>({1, 2, 3, 4, 5});
    {
        const auto ys = vector<double>({2, 1, 4, 3, 7});
        EXPECT_DOUBLE_EQ(matlab::corr(xs, ys), 0.8241633836921342);
    }
    {
        // Perfect positive and negative correlation.
        const auto ys = vector<double>({3, 5, 7, 9, 11});
        EXPECT_DOUBLE_EQ(matlab::corr(xs, ys), 1.0);
        const auto zs = vector<double>({11, 9, 7, 5, 3});
        EXPECT_DOUBLE_EQ(matlab::corr(xs, zs), -1.0);
    }
    {
        // A constant input has zero variance.
        const auto ys = vector<double>({2, 2, 2, 2, 2});
        EXPECT_TRUE(std::isnan(matlab::corr(xs, ys)));
    }
    {
        const auto ys = vector<double>({7.0});
        EXPECT_TRUE(std::isnan(matlab::corr(span<const double>(xs).first(1), ys)));
    }
}

TEST(matlab_statistics, corr2)
{
    const auto xs = {1.0, 3.0, 1.0, 4.0, 1.0, 5.0};
    const auto ys = {2.0, 5.0, 8.0, 5.0, 8.0, 4.0};
    const auto a = matlab::corr(xs, ys);
    EXPECT_NEAR(a, -0.340083758831913, 1e-15);
}
