#include "meadow/finance.h"

#include <gtest/gtest.h>

namespace
{
const vector<double> k_asset_returns{0.01, 0.02, -0.01, 0.03};
const vector<double> k_riskless_returns{0.001, 0.002, 0.001, 0.0015};

vector<double> toReturnFactors(span<const double> returns)
{
    vector<double> result;
    for (auto r : returns) {
        result.push_back(1.0 + r);
    }
    return result;
}
} // namespace

TEST(finance, sharpe_arithmetic)
{
    EXPECT_NEAR(sharpe(k_asset_returns, k_riskless_returns), 0.6635880662253102, 1e-12);
}

TEST(finance, sharpe_annualized)
{
    EXPECT_NEAR(sharpe(k_asset_returns, k_riskless_returns, 12.0), 2.298736491997236, 1e-12);
}

TEST(finance, sharpe_return_factor_input)
{
    const auto asset = toReturnFactors(k_asset_returns);
    const auto riskless = toReturnFactors(k_riskless_returns);
    EXPECT_NEAR(sharpe(asset, riskless, 1.0, SharpeInputType::return_factor), 0.6635880662253102, 1e-12);
}

TEST(finance, sharpe_geometric)
{
    EXPECT_NEAR(
      sharpe(k_asset_returns, k_riskless_returns, 1.0, SharpeInputType::return_, SharpeAggregation::geometric),
      0.658703604663648,
      1e-12
    );
}

TEST(finance, sharpe_constant_excess_return)
{
    const vector<double> asset{0.01, 0.01};
    const vector<double> riskless{0.001, 0.001};
    EXPECT_EQ(sharpe(asset, riskless), INFINITY);
}
