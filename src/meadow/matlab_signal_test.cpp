#include "matlab_butter_test_data.h"
#include "meadow/math.h"
#include "meadow/matlab_signal.h"
#include "meadow/span.h"

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
std::complex<double> eval_h(const matlab::TransferFunctionCoeffs& r, std::complex<double> z)
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
    // Gain at the bilinear-transformed analog center frequency must equal 1.
    for (int order : {1, 2, 3}) {
        const double wn1 = 0.2, wn2 = 0.6;
        auto r = matlab::butter(order, matlab::FilterType::BandPass{wn1, wn2});
        const double w1 = 2.0 * std::tan(std::numbers::pi * wn1 / 2.0);
        const double w2 = 2.0 * std::tan(std::numbers::pi * wn2 / 2.0);
        const double w0 = std::sqrt(w1 * w2);
        const std::complex<double> s_center = {0.0, w0};
        const auto z_center = (2.0 + s_center) / (2.0 - s_center);
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

TEST(matlab_signal, butter_reference_data)
{
    // Iterate over Octave-generated reference data (see matlab_butter_test_data.h).
    // Each group of 5 entries: args {order, low, high}, then concatenated b+a for LP, HP, BP, BS.
    constexpr double eps = 1e-8;
    const auto& d = matlab_butter_test_data;
    for (size_t i = 0; i < d.size(); i += 5) {
        const int order = static_cast<int>(d[i][0]);
        const double low = d[i][1];
        const double high = d[i][2];

        const struct {
            matlab::FilterType::V filter;
            size_t idx;      // index into the group (1..4)
            int coeff_order; // order used for coefficient count
        } cases[] = {
          {matlab::FilterType::LowPass{low},        1, order    },
          {matlab::FilterType::HighPass{low},       2, order    },
          {matlab::FilterType::BandPass{low, high}, 3, 2 * order},
          {matlab::FilterType::BandStop{low, high}, 4, 2 * order},
        };

        for (const auto& c : cases) {
            const auto& ref = d[i + c.idx];
            const size_t n = static_cast<size_t>(c.coeff_order + 1);
            ASSERT_EQ(ref.size(), 2 * n);

            const auto r = matlab::butter(order, c.filter);
            ASSERT_EQ(r.b.size(), n);
            ASSERT_EQ(r.a.size(), n);

            for (size_t k = 0; k < n; ++k) {
                EXPECT_NEAR(r.b[k], ref[k], eps) << "b[" << k << "] mismatch, group " << i / 5;
                EXPECT_NEAR(r.a[k], ref[n + k], eps) << "a[" << k << "] mismatch, group " << i / 5;
            }
        }
    }
}

template<class T>
using complex = std::complex<T>;

namespace
{
void assert_near(complex<double> a, complex<double> b, double eps)
{
    ASSERT_NEAR(std::real(a), std::real(b), eps);
    ASSERT_NEAR(std::imag(a), std::imag(b), eps);
}
} // namespace

TEST(matlab_signal, freqz)
{
    auto tf = matlab::butter(2, matlab::FilterType::LowPass{0.25});
    auto r0 = matlab::freqz(tf.b, tf.a, 0.0);
    auto r1 = matlab::freqz(tf.b, tf.a, num::pi * 0.125);
    auto r2 = matlab::freqz(tf.b, tf.a, num::pi * 0.25);
    auto r3 = matlab::freqz(tf.b, tf.a, num::pi * 0.375);
    const auto e0 = complex<double>(1, 0);
    const auto e1 = complex<double>(0.730541366470620, -0.644836760840970);
    const auto e2 = complex<double>(0.000000000000000, -0.707106781186547);
    const auto e3 = complex<double>(-0.206165241813622, -0.293554468133657);
    constexpr double eps = 1e-15;
    assert_near(r0, e0, eps);
    assert_near(r1, e1, eps);
    assert_near(r2, e2, eps);
    assert_near(r3, e3, eps);
}

// ---- bilinear ----------------------------------------------------------
// Reference values generated with MATLAB: [b,a] = bilinear(b_a, a_a, fs)
// and [b,a] = bilinear(b_a, a_a, fs, fp) for the prewarped cases.

TEST(matlab_signal, bilinear_order1_no_prewarp)
{
    // MATLAB: [b,a] = bilinear([1], [1,1], 44100)
    const double b_a[] = {1.0};
    const double a_a[] = {1.0, 1.0};
    auto r = matlab::bilinear(b_a, a_a, 44100.0, std::nullopt);
    ASSERT_EQ(r.b.size(), 2u);
    ASSERT_EQ(r.a.size(), 2u);
    constexpr double eps = 1e-15;
    expect_near(r.b, {1.13377399348912888399e-05, 1.13377399348912888399e-05}, eps);
    expect_near(r.a, {1.00000000000000000000e+00, -9.99977324520130106400e-01}, eps);
}

TEST(matlab_signal, bilinear_order2_no_prewarp)
{
    // MATLAB: [b,a] = bilinear([1], [1,sqrt(2),1], 8000)
    const double b_a[] = {1.0};
    const double a_a[] = {1.0, std::numbers::sqrt2, 1.0};
    auto r = matlab::bilinear(b_a, a_a, 8000.0, std::nullopt);
    ASSERT_EQ(r.b.size(), 3u);
    ASSERT_EQ(r.a.size(), 3u);
    constexpr double eps = 1e-14;
    expect_near(r.b, {3.90590471077700840397e-09, 7.81180964359862173296e-09, 3.90590459975470594145e-09}, eps);
    expect_near(r.a, {1.00000000000000000000e+00, -1.99982322330539386002e+00, 9.99823238929012925169e-01}, eps);
}

TEST(matlab_signal, bilinear_order2_nontrivial_numerator_no_prewarp)
{
    // MATLAB: [b,a] = bilinear([1,2], [1,3,2], 1000)
    const double b_a[] = {1.0, 2.0};
    const double a_a[] = {1.0, 3.0, 2.0};
    auto r = matlab::bilinear(b_a, a_a, 1000.0, std::nullopt);
    ASSERT_EQ(r.b.size(), 3u);
    ASSERT_EQ(r.a.size(), 3u);
    constexpr double eps = 1e-14;
    expect_near(r.b, {4.99750124937525797009e-04, 9.98501747950797380327e-07, -4.98751623189352955023e-04}, eps);
    expect_near(r.a, {1.00000000000000000000e+00, -1.99700249775212279602e+00, 9.97004494755619030677e-01}, eps);
}

TEST(matlab_signal, bilinear_order1_prewarp)
{
    // MATLAB: [b,a] = bilinear([1], [1,1], 8000, 1000)
    const double b_a[] = {1.0};
    const double a_a[] = {1.0, 1.0};
    auto r = matlab::bilinear(b_a, a_a, 8000.0, 1000.0);
    ASSERT_EQ(r.b.size(), 2u);
    ASSERT_EQ(r.a.size(), 2u);
    constexpr double eps = 1e-15;
    expect_near(r.b, {6.59197902421748693769e-05, 6.59197902421748693769e-05}, eps);
    expect_near(r.a, {1.00000000000000000000e+00, -9.99868160419515761284e-01}, eps);
}

TEST(matlab_signal, bilinear_order2_prewarp)
{
    // MATLAB: [b,a] = bilinear([1], [1,sqrt(2),1], 1000, 100)
    const double b_a[] = {1.0};
    const double a_a[] = {1.0, std::numbers::sqrt2, 1.0};
    auto r = matlab::bilinear(b_a, a_a, 1000.0, 100.0);
    ASSERT_EQ(r.b.size(), 3u);
    ASSERT_EQ(r.a.size(), 3u);
    constexpr double eps = 1e-14;
    expect_near(r.b, {2.67223550176609592199e-07, 5.34447100131174579474e-07, 2.67223550287631894662e-07}, eps);
    expect_near(r.a, {1.00000000000000000000e+00, -1.99853734787105885573e+00, 9.98538416765259451147e-01}, eps);
}

TEST(matlab_signal, mag2db)
{
    EXPECT_DOUBLE_EQ(matlab::mag2db(0.0), -INFINITY);
    EXPECT_DOUBLE_EQ(matlab::mag2db(1.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::mag2db(10.0), 20.0);
}

TEST(matlab_signal, pow2db)
{
    EXPECT_DOUBLE_EQ(matlab::pow2db(0.0), -INFINITY);
    EXPECT_DOUBLE_EQ(matlab::pow2db(1.0), 0.0);
    EXPECT_DOUBLE_EQ(matlab::pow2db(10.0), 10.0);
}

TEST(matlab_signal, db2mag)
{
    EXPECT_DOUBLE_EQ(matlab::db2mag(0.0), 1.0);
    EXPECT_DOUBLE_EQ(matlab::db2mag(20.0), 10.0);
}

TEST(matlab_signal, db2pow)
{
    EXPECT_DOUBLE_EQ(matlab::db2pow(0.0), 1.0);
    EXPECT_DOUBLE_EQ(matlab::db2pow(10.0), 10.0);
}

namespace
{
template<class Fn>
vector<double> make_win(size_t N, Fn&& fn)
{
    EXPECT_EQ(fn(-1), 0.0);
    EXPECT_EQ(fn(iicast<int>(N)), 0.0);
    vector<double> xs(N);
    for (size_t i : vi::iota(0u, N)) {
        xs[i] = fn(iicast<int>(i));
    }
    return xs;
}
} // namespace

TEST(matlab_signal, rectwin_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::rectwin_fn(n, 7);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::rectwin_fn(n, 8);
    });
    EXPECT_EQ(r7, vector<double>({1, 1, 1, 1, 1, 1, 1}));
    EXPECT_EQ(r8, vector<double>({1, 1, 1, 1, 1, 1, 1, 1}));
}

TEST(matlab_signal, blackman_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::blackman_fn(n, 7);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::blackman_fn(n, 8);
    });

    expect_near(
      r7,
      vector<double>(
        {-1.387778780781446e-17,
         1.299999999999999e-01,
         6.299999999999999e-01,
         9.999999999999999e-01,
         6.300000000000002e-01,
         1.299999999999999e-01,
         -1.387778780781446e-17}
      ),
      DBL_EPSILON
    );
    expect_near(
      r8,
      vector<double>(
        {-1.387778780781446e-17,
         9.045342435412804e-02,
         4.591829575459636e-01,
         9.203636180999081e-01,
         9.203636180999083e-01,
         4.591829575459638e-01,
         9.045342435412812e-02,
         -1.387778780781446e-17}
      ),
      DBL_EPSILON
    );
}

TEST(matlab_signal, gausswin_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::gausswin_fn(n, 7, 3.0);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::gausswin_fn(n, 8, 4.0);
    });

    expect_near(
      r7,
      vector<double>(
        {1.110899653824231e-02,
         1.353352832366127e-01,
         6.065306597126334e-01,
         1.000000000000000e+00,
         6.065306597126334e-01,
         1.353352832366127e-01,
         1.110899653824231e-02}
      ),
      DBL_EPSILON
    );
    expect_near(
      r8,
      vector<double>(
        {3.354626279025119e-04,
         1.687988414878993e-02,
         2.300662989938091e-01,
         8.493658165683124e-01,
         8.493658165683124e-01,
         2.300662989938091e-01,
         1.687988414878993e-02,
         3.354626279025119e-04}
      ),
      DBL_EPSILON
    );
}

#if MEADOW_HAS_CYL_BESSEL_I == 1 || MEADOW_HAS_BOOST == 1
TEST(matlab_signal, kaiser_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::kaiser_fn(n, 7, 1.23);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::kaiser_fn(n, 8, 1.78);
    });

    expect_near(
      r7,
      vector<double>(
        {0.706450386180156,
         0.862875634246974,
         0.964683201523233,
         1.000000000000000,
         0.964683201523233,
         0.862875634246974,
         0.706450386180156}
      ),
      1e-15
    );
    expect_near(
      r8,
      vector<double>(
        {0.509303391023423,
         0.726907890745535,
         0.895844495440142,
         0.988089123045320,
         0.988089123045320,
         0.895844495440142,
         0.726907890745535,
         0.509303391023423}
      ),
      1e-15
    );
}

TEST(matlab_signal, kaiser)
{
    for (int L : {7, 8}) {
        auto expected = make_win(sucast(L), [L](int n) {
            return matlab::kaiser_fn(n, L, 1.78);
        });
        expect_near(matlab::kaiser(L, 1.78), expected, 1e-15);
    }
}
#endif

TEST(matlab_signal, hann_poisson_fn)
{
    auto r7 = make_win(7, [](int n) {
        return matlab::hann_poisson_fn(n, 7, 2.1);
    });
    auto r8 = make_win(8, [](int n) {
        return matlab::hann_poisson_fn(n, 8, 2.9);
    });

    expect_near(
      r7,
      vector<double>(
        {0, 0.061649240985402, 0.372438977843557, 1.000000000000000, 0.372438977843557, 0.061649240985402, 0}
      ),
      1e-15
    );
    expect_near(
      r8,
      vector<double>(
        {0,
         0.023721205183369,
         0.176384455982313,
         0.628091634613610,
         0.628091634613610,
         0.176384455982313,
         0.023721205183369,
         0}
      ),
      1e-15
    );
}

TEST(matlab_signal, hann_poisson)
{
    for (int L : {7, 8}) {
        auto expected = make_win(sucast(L), [L](int n) {
            return matlab::hann_poisson_fn(n, L, 2.9);
        });
        expect_near(matlab::hann_poisson(L, 2.9), expected, 1e-15);
    }
}

TEST(matlab_signal, sinc)
{
    // Expected values for x = 10^i, i in [-10, -1] (sinc well away from zero)
    constexpr array k_expected = {
      1.000000000000000e+00,
      1.000000000000000e+00,
      9.999999999999998e-01,
      9.999999999999835e-01,
      9.999999999983551e-01,
      9.999999998355066e-01,
      9.999999835506593e-01,
      9.999983550667449e-01,
      9.998355147105485e-01,
      9.836316430834660e-01
    };

    static_assert(k_expected.size() == 10);
    ASSERT_EQ(matlab::sinc(0.0), 1.0);
    ASSERT_EQ(matlab::sinc(INFINITY), 0.0);
    ASSERT_EQ(matlab::sinc(-INFINITY), 0.0);
    for (int i = -10; i <= -1; ++i) {
        const auto x = pow(10, i);
        const auto actual = matlab::sinc(x);
        const auto expected = k_expected[sucast(i + 10)];
        ASSERT_LE(abs(log(actual / expected)), 1e-14);
    }
    // For integer x >= 1, sinc(x) = sin(pi*x)/(pi*x) is within floating-point noise of zero
    for (int i = 0; i <= 10; ++i) {
        ASSERT_LE(abs(matlab::sinc(pow(10, i))), 1e-15);
    }
}
