#include "meadow/matlab_statistics.h"
#include "meadow/matlab.h"

namespace matlab
{
double corr(span<const double> xs, span<const double> ys)
{
    // The normalization of the covariance matrix cancels out.
    const auto c = cov(xs, ys, 1);
    return c[0][1] / sqrt(c[0][0] * c[1][1]);
}
} // namespace matlab
