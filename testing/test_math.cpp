#include "meadow/math.h"

#include <gtest/gtest.h>

namespace
{
void expectDoubleEq(pair<double, double> a, pair<double, double> b)
{
    EXPECT_DOUBLE_EQ(a.first, b.first);
    EXPECT_DOUBLE_EQ(a.second, b.second);
}
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
