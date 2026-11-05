#include "meadow/enum.h"

#include <gtest/gtest.h>

TEST(enum, T1)
{
    enum class E {
        a,
        b,
        c
    };
    {
        Flags<E> fs;
        ASSERT_FALSE(fs.any());
        ASSERT_FALSE(fs.all());
    }
    {
        Flags<E> fs(E::b);
        ASSERT_TRUE(fs.any());
        ASSERT_FALSE(fs.all());
        ASSERT_FALSE(fs & E::a);
        ASSERT_TRUE(fs & E::b);
        ASSERT_FALSE(fs & E::c);
    }
    {
        auto fs = E::b | E::c;
        ASSERT_FALSE(fs & E::a);
        ASSERT_TRUE(fs & E::b);
        ASSERT_TRUE(fs & E::c);
    }
}
