#include "meadow/errno.h"

#include <gtest/gtest.h>

TEST(test_errno, strerrno_or_int)
{
    EXPECT_EQ(strerrno_or_int(ENXIO), "ENXIO");
    EXPECT_EQ(strerrno_or_int(12345), "12345");
}
