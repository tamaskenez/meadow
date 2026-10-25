#pragma once

#include "meadow/cppext.h"

namespace matlab
{
// Return Pearson's Linear Correlation Coefficient:
//     xs2 = xs - mean(xs);
//     ys2 = ys - mean(ys);
//     sum(xs2 .* ys2) / sqrt(sum(xs2.^2) * sum(ys2.^2))
// Precond: xs and ys have the same, non-zero size. Return NaN if either input is constant.
double corr(span<const double> xs, span<const double> ys);
} // namespace matlab
