#pragma once

#include <cassert>
#include <cmath>
#include <concepts>
#include <limits>

namespace detail
{
template<class R, class T>
    requires std::integral<R> && std::floating_point<T>
constexpr R float_to_int_cast_core(T fx)
{
    assert(
      !std::isnan(fx) && static_cast<T>(std::numeric_limits<R>::lowest()) <= fx
      && fx <= static_cast<T>(std::numeric_limits<R>::max())
    );
    return static_cast<R>(fx);
}
} // namespace detail
