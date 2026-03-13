#pragma once

#include <ranges>

#define DEFINE_RANGE_COMPOUND_ASSIGNMENT_OPERATOR(NAME, OPERATOR) \
    template<class XS, class YS>                                  \
    void NAME(XS&& xs, YS&& ys)                                   \
    {                                                             \
        const auto N = std::ranges::size(xs);                     \
        if constexpr (std::ranges::sized_range<YS>) {             \
            assert(std::ranges::size(ys) == N);                   \
            for (size_t i = 0; i < N; ++i) {                      \
                xs[i] OPERATOR ys[i];                             \
            }                                                     \
        } else {                                                  \
            for (size_t i = 0; i < N; ++i) {                      \
                xs[i] OPERATOR ys;                                \
            }                                                     \
        }                                                         \
    }

DEFINE_RANGE_COMPOUND_ASSIGNMENT_OPERATOR(range_plus_equals, +=)
DEFINE_RANGE_COMPOUND_ASSIGNMENT_OPERATOR(range_minus_equals, -=)
DEFINE_RANGE_COMPOUND_ASSIGNMENT_OPERATOR(range_multiply_equals, *=)
DEFINE_RANGE_COMPOUND_ASSIGNMENT_OPERATOR(range_divide_equals, /=)

#undef DEFINE_RANGE_COMPOUND_ASSIGNMENT_OPERATOR
