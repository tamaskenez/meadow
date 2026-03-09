#include "meadow/matlab_signal.h"
#include "meadow/matlab.h"

#include <cassert>
#include <cmath>
#include <complex>
#include <numbers>

#pragma clang diagnostic ignored "-Wsign-conversion"

namespace matlab
{

namespace
{

// Analog Butterworth prototype poles of order N, on the unit circle in the left half-plane.
// p_k = exp(j * π * (2k + N - 1) / (2N)), k = 1..N
std::vector<std::complex<double>> analog_proto_poles(int order)
{
    std::vector<std::complex<double>> poles(order);
    for (int k = 0; k < order; ++k) {
        double angle = std::numbers::pi * (2.0 * (k + 1) + order - 1) / (2.0 * order);
        poles[k] = std::polar(1.0, angle);
    }
    return poles;
}

// Bilinear transform: s-domain → z-domain (T = 1).
// z = (2 + s) / (2 - s)
std::complex<double> bilinear(std::complex<double> s)
{
    return (2.0 + s) / (2.0 - s);
}

// Build monic polynomial from roots: prod(z - r_k).
// Returns coefficients in descending powers of z: [1, c_{N-1}, ..., c_0].
std::vector<std::complex<double>> poly_from_roots(const std::vector<std::complex<double>>& roots)
{
    std::vector<std::complex<double>> poly = {1.0};
    for (auto r : roots) {
        std::vector<std::complex<double>> next(poly.size() + 1, 0.0);
        for (size_t i = 0; i < poly.size(); ++i) {
            next[i] += poly[i];
            next[i + 1] -= poly[i] * r;
        }
        poly = std::move(next);
    }
    return poly;
}

std::complex<double> polyval(const std::vector<std::complex<double>>& c, const std::complex<double>& z)
{
    return matlab::polyval(span<const std::complex<double>>(c), z);
}

std::vector<double> real_coeffs(const std::vector<std::complex<double>>& c)
{
    std::vector<double> r(c.size());
    for (size_t i = 0; i < c.size(); ++i)
        r[i] = c[i].real();
    return r;
}

// Assemble ButterworthCoeffs from digital zeros/poles and a reference point z_ref
// at which |H(z_ref)| is normalized to 1.
TransferFunctionCoeffs make_coeffs(
  const std::vector<std::complex<double>>& z_zeros,
  const std::vector<std::complex<double>>& z_poles,
  std::complex<double> z_ref
)
{
    auto b = poly_from_roots(z_zeros);
    auto a = poly_from_roots(z_poles);
    const double K = std::abs(polyval(a, z_ref) / polyval(b, z_ref));
    for (auto& c : b)
        c *= K;
    return {real_coeffs(b), real_coeffs(a)};
}

TransferFunctionCoeffs butter_lp(int order, double Wn)
{
    auto poles = analog_proto_poles(order);
    const double wc = 2.0 * std::tan(std::numbers::pi * Wn / 2.0);
    for (auto& p : poles)
        p *= wc;

    std::vector<std::complex<double>> z_poles(order), z_zeros(order, {-1.0, 0.0});
    for (int i = 0; i < order; ++i)
        z_poles[i] = bilinear(poles[i]);

    return make_coeffs(z_zeros, z_poles, {1.0, 0.0}); // normalize DC gain to 1
}

TransferFunctionCoeffs butter_hp(int order, double Wn)
{
    auto poles = analog_proto_poles(order);
    const double wc = 2.0 * std::tan(std::numbers::pi * Wn / 2.0);
    for (auto& p : poles)
        p = wc / p; // LP→HP: s → wc/s

    std::vector<std::complex<double>> z_poles(order), z_zeros(order, {1.0, 0.0});
    for (int i = 0; i < order; ++i)
        z_poles[i] = bilinear(poles[i]);

    return make_coeffs(z_zeros, z_poles, {-1.0, 0.0}); // normalize Nyquist gain to 1
}

TransferFunctionCoeffs butter_bp(int order, double Wn1, double Wn2)
{
    auto proto = analog_proto_poles(order);
    const double w1 = 2.0 * std::tan(std::numbers::pi * Wn1 / 2.0);
    const double w2 = 2.0 * std::tan(std::numbers::pi * Wn2 / 2.0);
    const double w0 = std::sqrt(w1 * w2);
    const double Bw = w2 - w1;

    // LP→BP: each s_k solves s² - s_k*Bw*s + w0² = 0 → two poles
    std::vector<std::complex<double>> z_poles;
    z_poles.reserve(2 * order);
    for (auto s : proto) {
        const auto q = s * (Bw / 2.0);
        const auto sq = std::sqrt(q * q - w0 * w0);
        z_poles.push_back(bilinear(q + sq));
        z_poles.push_back(bilinear(q - sq));
    }

    // N zeros at z=+1 (ω=0) and N zeros at z=-1 (ω=π)
    std::vector<std::complex<double>> z_zeros(2 * order);
    for (int i = 0; i < order; ++i)
        z_zeros[i] = {1.0, 0.0};
    for (int i = order; i < 2 * order; ++i)
        z_zeros[i] = {-1.0, 0.0};

    // Normalize at the bilinear-transformed analog center frequency
    const std::complex<double> s_center = {0.0, w0};
    const auto z_center = (2.0 + s_center) / (2.0 - s_center);
    return make_coeffs(z_zeros, z_poles, z_center);
}

TransferFunctionCoeffs butter_bs(int order, double Wn1, double Wn2)
{
    auto proto = analog_proto_poles(order);
    const double w1 = 2.0 * std::tan(std::numbers::pi * Wn1 / 2.0);
    const double w2 = 2.0 * std::tan(std::numbers::pi * Wn2 / 2.0);
    const double w0 = std::sqrt(w1 * w2);
    const double Bw = w2 - w1;

    // LP→BS: each s_k solves s_k*s² - Bw*s + s_k*w0² = 0
    // s = Bw/(2*s_k) ± sqrt((Bw/(2*s_k))² - w0²)
    std::vector<std::complex<double>> z_poles;
    z_poles.reserve(2 * order);
    for (auto s : proto) {
        const auto q = Bw / (2.0 * s);
        const auto sq = std::sqrt(q * q - w0 * w0);
        z_poles.push_back(bilinear(q + sq));
        z_poles.push_back(bilinear(q - sq));
    }

    // Transmission zeros at ±j*w0 in analog → conjugate pair on unit circle in z
    const auto z_notch = bilinear({0.0, w0});
    std::vector<std::complex<double>> z_zeros;
    z_zeros.reserve(2 * order);
    for (int i = 0; i < order; ++i) {
        z_zeros.push_back(z_notch);
        z_zeros.push_back(std::conj(z_notch));
    }

    return make_coeffs(z_zeros, z_poles, {1.0, 0.0}); // normalize DC gain to 1
}

std::vector<double> poly_mul(const std::vector<double>& p, const std::vector<double>& q)
{
    std::vector<double> result(p.size() + q.size() - 1, 0.0);
    for (size_t i = 0; i < p.size(); ++i) {
        for (size_t j = 0; j < q.size(); ++j) {
            result[i + j] += p[i] * q[j];
        }
    }
    return result;
}

} // namespace

TransferFunctionCoeffs butter(int order, const FilterType::V& filter)
{
    assert(order >= 1);
    return std::visit(
      [order](auto&& f) -> TransferFunctionCoeffs {
          using T = std::decay_t<decltype(f)>;
          if constexpr (std::is_same_v<T, FilterType::LowPass>)
              return butter_lp(order, f.cutoff);
          else if constexpr (std::is_same_v<T, FilterType::HighPass>)
              return butter_hp(order, f.cutoff);
          else if constexpr (std::is_same_v<T, FilterType::BandPass>)
              return butter_bp(order, f.low_cutoff, f.high_cutoff);
          else
              return butter_bs(order, f.low_cutoff, f.high_cutoff);
      },
      filter
    );
}

std::complex<double> freqz(std::span<const double> b, std::span<const double> a, double w)
{
    const std::complex<double> z_inv = std::exp(std::complex<double>(0.0, w));
    return polyval(b, z_inv) / polyval(a, z_inv);
}

TransferFunctionCoeffs
bilinear(std::span<const double> b, std::span<const double> a, double fs, std::optional<double> fp)
{
    // Prewarping factor λ: s = λ*(z-1)/(z+1).
    // With fp: match exact frequency response at fp Hz.
    // Without fp: standard bilinear (λ = 2*fs).
    const double lam =
      fp.has_value() ? 2.0 * std::numbers::pi * fp.value() / std::tan(std::numbers::pi * fp.value() / fs) : 2.0 * fs;

    const int nb = static_cast<int>(b.size());
    const int na = static_cast<int>(a.size());
    const int n = std::max(nb, na) - 1; // filter order

    // Pad numerator and denominator to the same order n (prepend zeros).
    std::vector<double> bp(n + 1, 0.0), ap(n + 1, 0.0);
    std::copy(b.begin(), b.end(), bp.begin() + (n + 1 - nb));
    std::copy(a.begin(), a.end(), ap.begin() + (n + 1 - na));

    // Precompute (z-1)^k and (z+1)^k for k = 0..n.
    std::vector<std::vector<double>> zm1(n + 1), zp1(n + 1);
    zm1[0] = zp1[0] = {1.0};
    for (int k = 1; k <= n; ++k) {
        zm1[k] = poly_mul(zm1[k - 1], {1.0, -1.0});
        zp1[k] = poly_mul(zp1[k - 1], {1.0, 1.0});
    }

    // Transform P(s) to a z-domain polynomial of degree n:
    // P_z(z) = sum_{k=0}^{n} p[n-k] * λ^k * (z-1)^k * (z+1)^(n-k)
    auto transform = [&](const std::vector<double>& p) {
        std::vector<double> result(n + 1, 0.0);
        double lam_k = 1.0;
        for (int k = 0; k <= n; ++k) {
            const auto term = poly_mul(zm1[k], zp1[n - k]);
            const double coeff = p[n - k] * lam_k;
            for (int i = 0; i <= n; ++i) {
                result[i] += coeff * term[i];
            }
            lam_k *= lam;
        }
        return result;
    };

    auto bz = transform(bp);
    auto az = transform(ap);

    // Normalize so that az[0] = 1.
    const double a0 = az[0];
    for (auto& c : bz) {
        c /= a0;
    }
    for (auto& c : az) {
        c /= a0;
    }

    return {bz, az};
}

} // namespace matlab
