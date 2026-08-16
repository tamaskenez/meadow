#pragma once
#include "meadow/cppext.h"

#include <span>

enum class SharpeInputType {
    return_,      // Flat return is 0.0
    return_factor // Flat return factor is 1.0
};
enum class SharpeAggregation {
    arithmetic,
    geometric
};

// Sharpe ratio of `asset` over the riskless asset.
// `asset` and `riskless` contain the per-period returns of the two assets, they must have the same size (>= 2).
// `periods_per_year` scales the result to a yearly ratio, leave it 1.0 to get the ratio for the period of the input.
// `aggregation` selects how the excess return of the asset is measured: `arithmetic` uses the difference of the
// returns, `geometric` the logarithm of the ratio of the return factors.
// Precond: the per-period excess returns must be finite, that is, the inputs must be finite and with
// `geometric` aggregation the return factors must also be positive. Asserts if the precondition is not met.
// Returns +/-infinity or NaN if the excess return is constant.
double sharpe(
  std::span<const double> asset,
  std::span<const double> riskless,
  double periods_per_year = 1.0,
  SharpeInputType input = SharpeInputType::return_,
  SharpeAggregation aggregation = SharpeAggregation::arithmetic
);
