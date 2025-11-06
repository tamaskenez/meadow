#pragma once

#include <variant>

/* Minimal helper for switching on std::variant without lambda function (switch_variant)
 *
 * Instead, we switch on a corresponding enum.
 * One way to organize types that go into a variant (an example):
 *
 * Say, we want to create a sum type named `Event` which can contain 3 alternatives:
 * `A`, `B` and `C`. We can organize it like this:
 *
 *     namespace Event {
 *         struct A {};
 *         struct B { int a; };
 *         struct C { double b; };
 *         using V = variant<A, B, C>;
 *     }
 *
 * It works, however, switching on the variant's current type is not convenient,
 * even with the `switch_variant` helper the debugging experience is much
 * worse than a normal `switch`.
 *
 * Here we introduce the "evariant" helper (enum-variant) which maintains
 * a parallel enum next to the variant. For a `switch` we convert variant::index()
 * into the enum and switch on that. To define the variant we need to use
 * a macro:
 *
 *     namespace Event {
 *         struct A {};
 *         struct B { int a; };
 *         struct C { double b; };
 *         EVARIANT_DECLARE_E_V(A, B, C);
 *     }
 *
 * The `EVARIANT_DECLARE_E_V` line declares the previous `V`, declares an enum `E`
 * with the values named identically as the alternative types and a helper function
 * `enum_of` (see below).
 *
 * Note: if you need an alternative type which is a built-on (e.g. `int`) you need
 * to add an alias for it:
 *
 *     using int_ = int;
 *
 * (end of note)
 *
 * To switch on the variant:
 *
 *     Event::V v = ...;
 *     switch(enum_of(v)) {
 *         case Event::E::A:
 *             break;
 *         case Event::E::B: {
 *             auto& x = std::get<Event::B>(v);
 *             int a = x.a;
 *         } break;
 *
 * For the latter case there's a helper macro:
 *
 *     switch(enum_of(v)) {
 *         case Event::E::A:
 *             break;
 *         EVARIANT_CASE(v, Event, B, x)
 *             int a = x.a;
 *         EVARIANT_BREAK
 *
 * In your .clang-format you need this to properly format the macro:
 *
 *     Macros:
 *     - EVARIANT_CASE(w, n, a, v)=case n:{
 *     - EVARIANT_BREAK=}
 *
 */

#define EVARIANT_DECLARE_E_V(...)        \
    using V = std::variant<__VA_ARGS__>; \
    enum class E {                       \
        __VA_ARGS__                      \
    };                                   \
    inline E enum_of(const V& v)         \
    {                                    \
        return E(v.index());             \
    }
#define EVARIANT_CASE(VARIANT_VAR, NAMESPACE, ALT, ALT_VAR) \
    case NAMESPACE::E::ALT: {                               \
        auto& ALT_VAR = std::get<NAMESPACE::ALT>(VARIANT_VAR);
#define EVARIANT_BREAK \
    }                  \
    break;
