#include "meadow/math.h"

std::pair<double, double> extremumOfParabola(double ym1, double y0, double yp1)
{
    const double a = (ym1 + yp1) / 2 - y0;
    const double b = (yp1 - ym1) / 2;
    const double x = -b / (2 * a);
    return std::pair(x, (a * x + b) * x + y0);
}
