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
