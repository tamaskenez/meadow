#include "meadow/finance.h"

#include "meadow/math.h"

#include <cmath>

namespace
{
double excessReturn(double asset, double riskless, SharpeInputType input, SharpeAggregation aggregation)
{
    const double asset_factor = input == SharpeInputType::return_factor ? asset : 1.0 + asset;
    const double riskless_factor = input == SharpeInputType::return_factor ? riskless : 1.0 + riskless;
    switch (aggregation) {
    case SharpeAggregation::arithmetic:
        return asset_factor - riskless_factor;
    case SharpeAggregation::geometric:
        return std::log(asset_factor / riskless_factor);
    }
    std::unreachable();
}
} // namespace

double sharpe(
  std::span<const double> asset,
  std::span<const double> riskless,
  double periods_per_year,
  SharpeInputType input,
  SharpeAggregation aggregation
)
{
    CHECK(asset.size() == riskless.size());
    CHECK(asset.size() >= 2);
    CHECK(periods_per_year > 0);

    RunningStat excess;
    for (size_t i = 0; i < asset.size(); ++i) {
        excess(excessReturn(asset[i], riskless[i], input, aggregation));
    }

    return excess.mean() / excess.stddev() * sqrt(periods_per_year);
}
