#pragma once

#include <cmath>
#include <concepts>

template<class T>
    requires std::floating_point<T>
T pow2db(T x)
{
    return static_cast<T>(10) * log10(x);
}
