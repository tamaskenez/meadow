#include "meadow/evariant.h"

#include <gtest/gtest.h>

namespace MyVariant
{
struct Choice1 {
};
struct Choice2 {
    int a;
};
struct Choice3 {
};
EVARIANT_DECLARE_E_V(Choice1, Choice2, Choice3)
} // namespace MyVariant

TEST(evariant, T1)
{
    MyVariant::V v = MyVariant::Choice2{3};

    optional<int> q;
    switch (enum_of(v)) {
    case MyVariant::E::Choice1:
        break;
        EVARIANT_CASE(v, MyVariant, Choice2, x)
        q = x.a;
    EVARIANT_BREAK
case MyVariant::E::Choice3:
    break;
}
ASSERT_EQ(q, 3);
}

TEST(evariant, T2)
{
    MyVariant::V v = MyVariant::Choice3{};

    optional<int> q;
    switch (enum_of(v)) {
    case MyVariant::E::Choice1:
        q = 0;
        break;
        EVARIANT_CASE(v, MyVariant, Choice2, x)
        q = x.a;
    EVARIANT_BREAK
case MyVariant::E::Choice3:
    q = 10;
    break;
}
ASSERT_EQ(q, 10);
}
