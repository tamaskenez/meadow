#include "meadow/matlab_signal.h"

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

std::complex<double> polyval(const std::vector<std::complex<double>>& c, std::complex<double> z)
{
    std::complex<double> result = 0;
    for (auto coeff : c)
        result = result * z + coeff;
    return result;
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
ButterworthCoeffs make_coeffs(
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

ButterworthCoeffs butter_lp(int order, double Wn)
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

ButterworthCoeffs butter_hp(int order, double Wn)
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

ButterworthCoeffs butter_bp(int order, double Wn1, double Wn2)
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

ButterworthCoeffs butter_bs(int order, double Wn1, double Wn2)
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

} // namespace

ButterworthCoeffs butter(int order, const FilterType::V& filter)
{
    assert(order >= 1);
    return std::visit(
      [order](auto&& f) -> ButterworthCoeffs {
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

} // namespace matlab
