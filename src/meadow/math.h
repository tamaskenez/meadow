#pragma once

#include <cmath>
#include <concepts>
#include <span>
#include <utility>

template<class T>
constexpr T square(T x)
{
    return x * x;
}

// Fit a parabola through (-1, ym1), (0, y0), (1, yp1) and return the point where the derivative is zero.
// Precond: (ym1 + yp1) / 2 - y0 != 0
std::pair<double, double> extremumOfParabola(double ym1, double y0, double yp1);

#if MEADOW_HAS_EIGEN == 1
// Fit a parabola through xs, ys and return the point where the derivative is zero.
template<class R, class X, class Y>
std::pair<R, R> extremumOfParabola(std::span<const X> xs, std::span<const Y> ys);
#endif

template<class T>
    requires std::integral<T>
bool isEven(T x)
{
    return (x & 1) == 0;
}

template<class T>
    requires std::integral<T>
bool isOdd(T x)
{
    return (x & 1) == 1;
}

template<class T>
    requires std::floating_point<T>
T sgn(T x)
{
    switch (std::fpclassify(x)) {
    case FP_NAN:
    case FP_ZERO:
        return x;
    default:
        return std::copysign(T(1), x);
    }
}

template<class T>
    requires std::signed_integral<T>
T sgn(T x)
{
    return x > T(0) ? T(1) : x >> (sizeof(T) * 8 - 1);
}

template<class T>
    requires std::unsigned_integral<T>
T sgn(T x)
{
    return x > T(0) ? T(1) : T(0);
}
