#include "meadow/dsp/PowerFilterBank.h"

#include "meadow/math.h"

#include <algorithm>

#include <cmath>
#include <complex>
#include <type_traits>

namespace
{
// States below this are flushed to zero at the end of each block, to avoid denormals in long silences. About the
// square root of the smallest normal number.
template<class T>
constexpr T k_flush_to_zero_threshold = std::is_same_v<T, float> ? T(1e-19) : T(1e-150);
// Upper limit of the power low-pass cutoff frequency, relative to the sample rate.
constexpr double k_max_lpf_freq_fs_ratio = 0.45;

template<class T>
void flush_to_zero(T& x)
{
    if (std::abs(x) < k_flush_to_zero_threshold<T>) {
        x = 0;
    }
}
} // namespace

template<class IOFloat, class ProcFloat>
PowerFilterBank<IOFloat, ProcFloat>::PowerFilterBank(
  double lo_freq_hz,
  double hi_freq_hz,
  double bands_per_octave,
  int bp_filter_order,
  double Q,
  double lpf_freq_ratio_arg,
  double max_lpf_freq_hz_arg
)
    : lpf_freq_ratio(lpf_freq_ratio_arg)
    , max_lpf_freq_hz(max_lpf_freq_hz_arg)
{
    CHECK(0 < lo_freq_hz && lo_freq_hz <= hi_freq_hz)
      << format("PowerFilterBank: invalid frequency range: lo = {} Hz, hi = {} Hz", lo_freq_hz, hi_freq_hz);
    CHECK(bands_per_octave > 0) << format("PowerFilterBank: invalid bands_per_octave: {}", bands_per_octave);
    CHECK(2 <= bp_filter_order && bp_filter_order <= k_max_bp_filter_order && bp_filter_order % 2 == 0) << format(
      "PowerFilterBank: invalid bp_filter_order: {}, must be even, between 2 and {}",
      bp_filter_order,
      k_max_bp_filter_order
    );
    CHECK(Q > 0.5) << format("PowerFilterBank: invalid Q: {}, must be > 0.5", Q);
    CHECK(lpf_freq_ratio > 0) << format("PowerFilterBank: invalid lpf_freq_ratio: {}", lpf_freq_ratio);
    CHECK(max_lpf_freq_hz > 0) << format("PowerFilterBank: invalid max_lpf_freq_hz: {}", max_lpf_freq_hz);

    const auto num_bands =
      iicast<size_t>(ifloor<int>(std::log2(hi_freq_hz / lo_freq_hz) * bands_per_octave + 1e-9)) + 1;
    band_freqs_var.resize(num_bands);
    for (size_t k = 0; k < num_bands; ++k) {
        band_freqs_var[k] = lo_freq_hz * std::exp2(ifcast<double>(k) / bands_per_octave);
    }
    band_powers_var.assign(num_bands, ProcFloat(0));
    bands.resize(num_bands);

    // Analog Butterworth lowpass prototype of order n, transformed to a bandpass centered at 1 rad/s by substituting
    // s -> Q * (s + 1/s). Each prototype pole p yields the bandpass poles solving s² - (p/Q) s + 1 = 0. A complex
    // conjugate pole pair of the prototype yields two conjugate pole pairs, a real pole yields one: n sections total.
    const int n = bp_filter_order / 2;
    num_sections = iicast<size_t>(n);
    auto add_section = [this, i = size_t(0)](std::complex<double> pole) mutable {
        const auto w = std::abs(pole);
        analog_sections[i++] = AnalogSection{.w = w, .R2 = -2.0 * pole.real() / w};
    };
    for (int k = 1; 2 * k <= n + 1; ++k) {
        const auto p = std::polar(1.0, num::pi * ifcast<double>(2 * k + n - 1) / ifcast<double>(2 * n));
        const auto a = p / (2.0 * Q);
        const auto r = std::sqrt(a * a - 1.0);
        if (2 * k == n + 1) {
            // Real prototype pole, the two bandpass poles are a conjugate pair.
            add_section(a + r);
        } else {
            add_section(a + r);
            add_section(a - r);
        }
    }

    // Normalize to unity gain at the band frequency. Each section is the SVF bandpass s'/(s'² + R2 s' + 1), s' = s/w.
    std::complex<double> h_center = 1.0;
    for (size_t i = 0; i < num_sections; ++i) {
        const auto& as = analog_sections[i];
        const std::complex<double> s(0.0, 1.0 / as.w);
        h_center *= s / (s * s + as.R2 * s + 1.0);
    }
    analog_power_gain = 1.0 / std::norm(h_center);
}

template<class IOFloat, class ProcFloat>
double PowerFilterBank<IOFloat, ProcFloat>::Q_for_bands_per_octave(double bands_per_octave)
{
    const auto half_band_ratio = std::exp2(0.5 / bands_per_octave);
    return 1.0 / (half_band_ratio - 1.0 / half_band_ratio);
}

template<class IOFloat, class ProcFloat>
void PowerFilterBank<IOFloat, ProcFloat>::prepare_to_play(double sample_rate_arg)
{
    CHECK(sample_rate_arg > 0) << format("PowerFilterBank: invalid sample rate: {}", sample_rate_arg);
    sample_rate = sample_rate_arg;
    num_active_bands = 0;
    while (num_active_bands < bands.size()
           && band_freqs_var[num_active_bands] < k_max_band_freq_nyquist_ratio * sample_rate / 2) {
        ++num_active_bands;
    }
    for (size_t k = 0; k < num_active_bands; ++k) {
        const auto f = band_freqs_var[k];
        auto& b = bands[k];
        const auto Omega0 = std::tan(num::pi * f / sample_rate);
        for (size_t i = 0; i < num_sections; ++i) {
            const auto& as = analog_sections[i];
            auto& s = b.sections[i];
            const auto g = Omega0 * as.w;
            s.g = ffcast<ProcFloat>(g);
            s.R2_plus_g = ffcast<ProcFloat>(as.R2 + g);
            s.h = ffcast<ProcFloat>(1.0 / (1.0 + as.R2 * g + g * g));
        }
        b.power_gain = ffcast<ProcFloat>(analog_power_gain);
        const auto lpf_freq = std::min({f / lpf_freq_ratio, max_lpf_freq_hz, k_max_lpf_freq_fs_ratio * sample_rate});
        const auto g = std::tan(num::pi * lpf_freq / sample_rate);
        b.lpf_G = ffcast<ProcFloat>(g / (1.0 + g));
    }
    reset();
}

template<class IOFloat, class ProcFloat>
void PowerFilterBank<IOFloat, ProcFloat>::reset()
{
    for (auto& b : bands) {
        for (auto& s : b.sections) {
            s.s1 = 0;
            s.s2 = 0;
        }
        b.lpf_s = 0;
    }
    std::ranges::fill(band_powers_var, ProcFloat(0));
}

template<class IOFloat, class ProcFloat>
void PowerFilterBank<IOFloat, ProcFloat>::set_lanes(size_t lanes)
{
    CHECK(lanes == 0 || std::ranges::contains(k_supported_lanes, lanes))
      << format("PowerFilterBank: unsupported number of lanes: {}", lanes);
    lanes_override = lanes;
}

template<class IOFloat, class ProcFloat>
void PowerFilterBank<IOFloat, ProcFloat>::process_block(std::span<const IOFloat> samples)
{
    assert(!std::isnan(sample_rate));
    if (samples.empty()) {
        return;
    }
    switch (num_sections) {
    case 1:
        process_block_with_num_sections<1>(samples);
        break;
    case 2:
        process_block_with_num_sections<2>(samples);
        break;
    case 3:
        process_block_with_num_sections<3>(samples);
        break;
    case 4:
        process_block_with_num_sections<4>(samples);
        break;
    default:
        assert(false);
    }
}

template<class IOFloat, class ProcFloat>
template<size_t NS>
void PowerFilterBank<IOFloat, ProcFloat>::process_block_with_num_sections(std::span<const IOFloat> samples)
{
    static_assert(NS <= k_max_sections);
    const auto lanes = lanes_override != 0 ? lanes_override : k_default_lanes[NS - 1];
    auto process_all = [&]<size_t Lanes>() {
        for (size_t first_band = 0; first_band < num_active_bands; first_band += Lanes) {
            process_bands<NS, Lanes>(samples, first_band);
        }
    };
    switch (lanes) {
    case 1:
        process_all.template operator()<1>();
        break;
    case 4:
        process_all.template operator()<4>();
        break;
    case 8:
        process_all.template operator()<8>();
        break;
    case 16:
        process_all.template operator()<16>();
        break;
    case 32:
        process_all.template operator()<32>();
        break;
    case 64:
        process_all.template operator()<64>();
        break;
    default:
        assert(false);
    }
}

template<class IOFloat, class ProcFloat>
template<size_t NS, size_t Lanes>
void PowerFilterBank<IOFloat, ProcFloat>::process_bands(std::span<const IOFloat> samples, size_t first_band)
{
    // Structure-of-arrays copies of the bands, the innermost loops run over the lanes so the compiler can vectorize
    // them. The dummy lanes have zero coefficients and stay zero.
    struct SectionLanes {
        array<ProcFloat, Lanes> g{}, R2_plus_g{}, h{}, s1{}, s2{};
    };
    array<SectionLanes, NS> sections;
    array<ProcFloat, Lanes> power_gain{}, lpf_G{}, lpf_s{}, power{};
    const auto num_lanes = std::min(Lanes, num_active_bands - first_band);
    for (size_t l = 0; l < num_lanes; ++l) {
        const auto& b = bands[first_band + l];
        for (size_t i = 0; i < NS; ++i) {
            const auto& s = b.sections[i];
            auto& sl = sections[i];
            sl.g[l] = s.g;
            sl.R2_plus_g[l] = s.R2_plus_g;
            sl.h[l] = s.h;
            sl.s1[l] = s.s1;
            sl.s2[l] = s.s2;
        }
        power_gain[l] = b.power_gain;
        lpf_G[l] = b.lpf_G;
        lpf_s[l] = b.lpf_s;
    }

    for (const auto x_in : samples) {
        array<ProcFloat, Lanes> x;
        x.fill(ffcast<ProcFloat>(x_in));
        for (auto& sl : sections) {
            for (size_t l = 0; l < Lanes; ++l) {
                const auto y_hp = sl.h[l] * (x[l] - sl.R2_plus_g[l] * sl.s1[l] - sl.s2[l]);
                const auto y_bp = sl.g[l] * y_hp + sl.s1[l];
                sl.s1[l] = 2 * y_bp - sl.s1[l];
                const auto y_lp = sl.g[l] * y_bp + sl.s2[l];
                sl.s2[l] = 2 * y_lp - sl.s2[l];
                x[l] = y_bp;
            }
        }
        for (size_t l = 0; l < Lanes; ++l) {
            const auto v = (power_gain[l] * x[l] * x[l] - lpf_s[l]) * lpf_G[l];
            power[l] = v + lpf_s[l];
            lpf_s[l] = power[l] + v;
        }
    }

    for (size_t l = 0; l < num_lanes; ++l) {
        auto& b = bands[first_band + l];
        for (size_t i = 0; i < NS; ++i) {
            auto& s = b.sections[i];
            const auto& sl = sections[i];
            s.s1 = sl.s1[l];
            s.s2 = sl.s2[l];
            flush_to_zero(s.s1);
            flush_to_zero(s.s2);
        }
        b.lpf_s = lpf_s[l];
        flush_to_zero(b.lpf_s);
        band_powers_var[first_band + l] = power[l];
    }
}

template class PowerFilterBank<float, float>;
template class PowerFilterBank<float, double>;
template class PowerFilterBank<double, float>;
template class PowerFilterBank<double, double>;
