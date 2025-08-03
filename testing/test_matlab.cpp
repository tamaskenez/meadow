#include "meadow/matlab.h"

#include <gtest/gtest.h>

TEST(matlab, mag2db)
{
    EXPECT_DOUBLE_EQ(matlab::mag2db(1.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::mag2db(10.0), 20.0);
}

TEST(matlab, pow2db)
{
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
