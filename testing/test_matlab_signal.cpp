#include "meadow/matlab_signal.h"

#include <gtest/gtest.h>

#include <cmath>
#include <complex>
#include <numbers>

namespace
{

void expect_near(const std::vector<double>& x, const std::vector<double>& y, double e)
{
    ASSERT_EQ(x.size(), y.size());
    for (size_t i = 0; i < x.size(); ++i)
        EXPECT_NEAR(x[i], y[i], e);
}

// Evaluate H(z) = sum_b / sum_a for z = exp(j*w).
std::complex<double> eval_h(const matlab::ButterworthCoeffs& r, std::complex<double> z)
{
    std::complex<double> num = 0, den = 0;
    for (auto c : r.b)
        num = num * z + c;
    for (auto c : r.a)
        den = den * z + c;
    return num / den;
}

} // namespace

// ---- Low-pass ----------------------------------------------------------

TEST(matlab_signal, butter_lp_order1)
{
    // MATLAB: [b,a] = butter(1, 0.5)
    // Analog pole: -1, wc=2, bilinear → z_pole=0, z_zero=-1, K=0.5
    auto r = matlab::butter(1, matlab::FilterType::LowPass{0.5});
    ASSERT_EQ(r.b.size(), 2u);
    ASSERT_EQ(r.a.size(), 2u);
    EXPECT_DOUBLE_EQ(r.a[0], 1.0);
    expect_near(r.b, {0.5, 0.5}, 1e-15);
    expect_near(r.a, {1.0, 0.0}, 1e-15);
}

TEST(matlab_signal, butter_lp_order2)
{
    // MATLAB: [b,a] = butter(2, 0.5)
    // Exact: b = [1-1/√2, 2-√2, 1-1/√2], a = [1, 0, 3-2√2]
    const double s2 = std::numbers::sqrt2;
    auto r = matlab::butter(2, matlab::FilterType::LowPass{0.5});
    ASSERT_EQ(r.b.size(), 3u);
    ASSERT_EQ(r.a.size(), 3u);
    expect_near(r.b, {1.0 - 1.0 / s2, 2.0 - s2, 1.0 - 1.0 / s2}, 1e-14);
    expect_near(r.a, {1.0, 0.0, 3.0 - 2.0 * s2}, 1e-14);
}

TEST(matlab_signal, butter_lp_dc_gain)
{
    // DC gain (z=1) must equal 1 for a low-pass filter.
    for (int order : {1, 2, 3, 4}) {
        for (double wn : {0.1, 0.3, 0.5, 0.7}) {
            auto r = matlab::butter(order, matlab::FilterType::LowPass{wn});
            EXPECT_NEAR(std::abs(eval_h(r, {1.0, 0.0})), 1.0, 1e-12);
        }
    }
}

TEST(matlab_signal, butter_lp_nyquist_attenuation)
{
    // A low-pass filter must attenuate the Nyquist frequency (z=-1).
    for (int order : {1, 2, 3, 4}) {
        for (double wn : {0.1, 0.3, 0.5}) {
            auto r = matlab::butter(order, matlab::FilterType::LowPass{wn});
            EXPECT_LT(std::abs(eval_h(r, {-1.0, 0.0})), 0.5);
        }
    }
}

TEST(matlab_signal, butter_lp_coeffs_size)
{
    for (int order : {1, 2, 3, 5}) {
        auto r = matlab::butter(order, matlab::FilterType::LowPass{0.3});
        EXPECT_EQ(r.b.size(), static_cast<size_t>(order + 1));
        EXPECT_EQ(r.a.size(), static_cast<size_t>(order + 1));
        EXPECT_DOUBLE_EQ(r.a[0], 1.0);
    }
}

// ---- High-pass ---------------------------------------------------------

TEST(matlab_signal, butter_hp_order1)
{
    // MATLAB: [b,a] = butter(1, 0.5, 'high')
    // By Wn=0.5 symmetry: same pole location as LP, zeros at z=+1, K=0.5
    auto r = matlab::butter(1, matlab::FilterType::HighPass{0.5});
    ASSERT_EQ(r.b.size(), 2u);
    ASSERT_EQ(r.a.size(), 2u);
    expect_near(r.b, {0.5, -0.5}, 1e-15);
    expect_near(r.a, {1.0, 0.0}, 1e-15);
}

TEST(matlab_signal, butter_hp_order2)
{
    // MATLAB: [b,a] = butter(2, 0.5, 'high')
    // Exact: b = [1-1/√2, -(2-√2), 1-1/√2], a = [1, 0, 3-2√2]
    const double s2 = std::numbers::sqrt2;
    auto r = matlab::butter(2, matlab::FilterType::HighPass{0.5});
    ASSERT_EQ(r.b.size(), 3u);
    ASSERT_EQ(r.a.size(), 3u);
    expect_near(r.b, {1.0 - 1.0 / s2, -(2.0 - s2), 1.0 - 1.0 / s2}, 1e-14);
    expect_near(r.a, {1.0, 0.0, 3.0 - 2.0 * s2}, 1e-14);
}

TEST(matlab_signal, butter_hp_nyquist_gain)
{
    // Nyquist gain (z=-1) must equal 1 for a high-pass filter.
    for (int order : {1, 2, 3, 4}) {
        for (double wn : {0.3, 0.5, 0.7}) {
            auto r = matlab::butter(order, matlab::FilterType::HighPass{wn});
            EXPECT_NEAR(std::abs(eval_h(r, {-1.0, 0.0})), 1.0, 1e-12);
        }
    }
}

TEST(matlab_signal, butter_hp_dc_attenuation)
{
    // A high-pass filter must attenuate DC (z=1).
    for (int order : {1, 2, 3}) {
        for (double wn : {0.5, 0.7}) {
            auto r = matlab::butter(order, matlab::FilterType::HighPass{wn});
            EXPECT_LT(std::abs(eval_h(r, {1.0, 0.0})), 0.5);
        }
    }
}

TEST(matlab_signal, butter_hp_coeffs_size)
{
    for (int order : {1, 2, 3, 5}) {
        auto r = matlab::butter(order, matlab::FilterType::HighPass{0.3});
        EXPECT_EQ(r.b.size(), static_cast<size_t>(order + 1));
        EXPECT_EQ(r.a.size(), static_cast<size_t>(order + 1));
        EXPECT_DOUBLE_EQ(r.a[0], 1.0);
    }
}

// ---- Band-pass ---------------------------------------------------------

TEST(matlab_signal, butter_bp_coeffs_size)
{
    // BP of order N produces a 2N-th order filter.
    for (int order : {1, 2, 3}) {
        auto r = matlab::butter(order, matlab::FilterType::BandPass{0.2, 0.6});
        EXPECT_EQ(r.b.size(), static_cast<size_t>(2 * order + 1));
        EXPECT_EQ(r.a.size(), static_cast<size_t>(2 * order + 1));
        EXPECT_DOUBLE_EQ(r.a[0], 1.0);
    }
}

TEST(matlab_signal, butter_bp_center_gain)
{
    // Gain at the geometric center of the passband must equal 1.
    for (int order : {1, 2, 3}) {
        const double wn1 = 0.2, wn2 = 0.6;
        auto r = matlab::butter(order, matlab::FilterType::BandPass{wn1, wn2});
        const auto z_center = std::polar(1.0, std::numbers::pi * std::sqrt(wn1 * wn2));
        EXPECT_NEAR(std::abs(eval_h(r, z_center)), 1.0, 1e-12);
    }
}

TEST(matlab_signal, butter_bp_stopband_attenuation)
{
    // DC and Nyquist must be in the stopband.
    auto r = matlab::butter(2, matlab::FilterType::BandPass{0.2, 0.6});
    EXPECT_LT(std::abs(eval_h(r, {1.0, 0.0})), 0.01);  // DC
    EXPECT_LT(std::abs(eval_h(r, {-1.0, 0.0})), 0.01); // Nyquist
}

// ---- Band-stop ---------------------------------------------------------

TEST(matlab_signal, butter_bs_coeffs_size)
{
    // BS of order N produces a 2N-th order filter.
    for (int order : {1, 2, 3}) {
        auto r = matlab::butter(order, matlab::FilterType::BandStop{0.2, 0.6});
        EXPECT_EQ(r.b.size(), static_cast<size_t>(2 * order + 1));
        EXPECT_EQ(r.a.size(), static_cast<size_t>(2 * order + 1));
        EXPECT_DOUBLE_EQ(r.a[0], 1.0);
    }
}

TEST(matlab_signal, butter_bs_dc_gain)
{
    // DC gain must equal 1 for a band-stop filter.
    for (int order : {1, 2, 3}) {
        auto r = matlab::butter(order, matlab::FilterType::BandStop{0.2, 0.6});
        EXPECT_NEAR(std::abs(eval_h(r, {1.0, 0.0})), 1.0, 1e-12);
    }
}

TEST(matlab_signal, butter_bs_notch_attenuation)
{
    // Gain at the notch frequency must be near zero.
    // The notch is at bilinear(j*w0) where w0 = sqrt(2*tan(π*wn1/2) * 2*tan(π*wn2/2)).
    for (int order : {2, 3}) {
        const double wn1 = 0.2, wn2 = 0.6;
        auto r = matlab::butter(order, matlab::FilterType::BandStop{wn1, wn2});
        const double w1 = 2.0 * std::tan(std::numbers::pi * wn1 / 2.0);
        const double w2 = 2.0 * std::tan(std::numbers::pi * wn2 / 2.0);
        const double w0 = std::sqrt(w1 * w2);
        // bilinear(j*w0) = (2 + j*w0) / (2 - j*w0)
        const std::complex<double> s_notch = {0.0, w0};
        const auto z_notch = (2.0 + s_notch) / (2.0 - s_notch);
        EXPECT_LT(std::abs(eval_h(r, z_notch)), 1e-10);
    }
}
