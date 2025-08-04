#pragma once

#include <utility>

template<class T>
T square(T x)
{
    return x * x;
}

// Fit a parabola through (-1, ym1), (0, y0), (1, yp1) and return the point where the derivative is zero.
// Precond: (ym1 + yp1) / 2 - y0 != 0
std::pair<double, double> extremumOfParabola(double ym1, double y0, double yp1);

// Fit a parabola through xs, ys and return the point where the derivative is zero.
template<class R, class X, class Y>
std::pair<R, R> extremumOfParabola(span<const X> xs, span<const Y> ys);

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
