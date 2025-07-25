#include "meadow/math.h"

#include <gtest/gtest.h>

namespace
{
void expectDoubleEq(pair<double, double> a, pair<double, double> b)
{
    EXPECT_DOUBLE_EQ(a.first, b.first);
    EXPECT_DOUBLE_EQ(a.second, b.second);
}

pair<double, double> callExtremumOfParabola(initializer_list<double> xs, initializer_list<double> ys)
{
    vector<double> vxs(xs);
    vector<double> vys(ys);
    return extremumOfParabola<double>(span(vxs.data(), vxs.size()), span(vys.data(), vys.size()));
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
}
