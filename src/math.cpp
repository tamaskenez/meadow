#include "meadow/math.h"

std::pair<double, double> extremumOfParabola(double ym1, double y0, double yp1)
{
    const double a = (ym1 + yp1) / 2 - y0;
    const double b = (yp1 - ym1) / 2;
    const double x = -b / (2 * a);
    return std::pair(x, (a * x + b) * x + y0);
}

template<class T>
std::pair<T, T> extremumOfParabola(span<const T> xs, span<const T> ys)
{
    assert(xs.size() == ys.size());
    assert(xs.size() >= 3);
    assert(xs.size() == 3);
    const double x0 = double(xs[0]) - double(xs[1]);
    const double x2 = double(xs[2]) - double(xs[1]);
    const double y0 = double(ys[0]) - double(ys[1]);
    const double y2 = double(ys[2]) - double(ys[1]);

    const double D = square(x0) * x2 - x0 * square(x2);

    const double a = (x2 * y0 - x0 * y2) / D;
    const double b = (-square(x2) * y0 + square(x0) * y2) / D;

    const double x = -b / (2 * a);
    const double y = (a * x + b) * x;
    return std::pair(ffcast<T>(x + xs[1]), ffcast<T>(y + ys[1]));
}

template std::pair<float, float> extremumOfParabola(span<const float> xs, span<const float> ys);
template std::pair<double, double> extremumOfParabola(span<const double> xs, span<const double> ys);
