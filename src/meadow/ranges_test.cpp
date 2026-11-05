#include "meadow/ranges.h"

#include <gtest/gtest.h>

TEST(ranges, divide_equals)
{
    std::vector<int> xs0 = {27, 12, 30};
    const std::vector<int> ys = {3, 4, 5};

    {
        auto xs = xs0;
        range_divide_equals(xs, ys);
        for (unsigned i = 0; i < xs.size(); ++i) {
            EXPECT_EQ(xs[i], xs0[i] / ys[i]);
        }
    }

    {
        auto xs = xs0;
        range_divide_equals(std::span(xs), std::span(ys));
        for (unsigned i = 0; i < xs.size(); ++i) {
            EXPECT_EQ(xs[i], xs0[i] / ys[i]);
        }
    }

    {
        auto xs = xs0;
        range_divide_equals(xs, 3);
        for (unsigned i = 0; i < xs.size(); ++i) {
            EXPECT_EQ(xs[i], xs0[i] / 3);
        }
    }

    {
        auto xs = xs0;
        range_divide_equals(std::span(xs), 3);
        for (unsigned i = 0; i < xs.size(); ++i) {
            EXPECT_EQ(xs[i], xs0[i] / 3);
        }
    }
}
