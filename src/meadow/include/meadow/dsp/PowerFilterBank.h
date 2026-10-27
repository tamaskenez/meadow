#pragma once

#include <meadow/cppext.h>

#include <span>

// Bank of logarithmically spaced bandpass filters, each followed by squaring and a 1st order low-pass, yielding a
// running power estimate per band.
//
// Each bandpass is a Butterworth bandpass of order `bp_filter_order` (the lowpass-to-bandpass transform of a
// Butterworth lowpass of order bp_filter_order/2), implemented as a cascade of TPT state-variable filter sections and
// normalized to unity gain at the band frequency. `Q` is the Q of the whole bandpass: band frequency / -3 dB bandwidth,
// so it is independent of the order and of the band spacing. The design is prewarped at the band frequency, which is
// exact, but the bandwidth of bands close to Nyquist will be narrower than specified.
//
// A sine of amplitude A at a band frequency yields A²/2 in that band.
//
// The coefficients are calculated in double, `process_block` works in `ProcFloat`.
template<class IOFloat, class ProcFloat = double>
class PowerFilterBank
{
public:
    static constexpr int k_max_bp_filter_order = 8;

    // Band frequencies: lo_freq_hz * 2^(k / bands_per_octave) for k = 0, 1, ..., as long as it's <= hi_freq_hz.
    // `bp_filter_order` must be even, 2 <= bp_filter_order <= k_max_bp_filter_order.
    // Cutoff frequency of the power low-pass of each band: min(band_freq / lpf_freq_ratio, max_lpf_freq_hz).
    // Allocates.
    PowerFilterBank(
      double lo_freq_hz,
      double hi_freq_hz,
      double bands_per_octave,
      int bp_filter_order,
      double Q,
      double lpf_freq_ratio,
      double max_lpf_freq_hz
    );

    // Q of a bandpass whose -3 dB band edges are the geometric midpoints between adjacent band frequencies.
    NODIS static double Q_for_bands_per_octave(double bands_per_octave);

    // Computes the coefficients and resets the state. Bands at or above k_max_band_freq_nyquist_ratio * Nyquist are
    // inactive: they always report zero power. Doesn't allocate.
    void prepare_to_play(double sample_rate);
    void reset();

    void process_block(std::span<const IOFloat> samples);

    // `process_block` processes this many bands in parallel, depending on the number of sections. Measured with
    // PowerFilterBank.DISABLED_benchmark_lanes, float, 12 bands/octave. The best values depend mostly on the compiler.
#if defined(_MSC_VER) && !defined(__clang__)
    // MSVC, x86-64 (GitHub Actions windows-2025).
    static constexpr array<size_t, k_max_bp_filter_order / 2> k_default_lanes = {32, 8, 8, 8};
#elif defined(__GNUC__) && !defined(__clang__)
    // GCC 16, x86-64 (GitHub Actions ubuntu-26.04).
    static constexpr array<size_t, k_max_bp_filter_order / 2> k_default_lanes = {64, 64, 32, 64};
#else
    // Apple Clang, arm64 (Apple M5, GitHub Actions macos-26). Not measured with Clang on x86-64.
    static constexpr array<size_t, k_max_bp_filter_order / 2> k_default_lanes = {32, 16, 8, 8};
#endif
    static constexpr array<size_t, 6> k_supported_lanes = {1, 4, 8, 16, 32, 64};
    // Overrides k_default_lanes, for benchmarking. `lanes` must be one of k_supported_lanes, or 0 to restore the
    // default.
    void set_lanes(size_t lanes);

    NODIS size_t num_bands() const
    {
        return band_freqs_var.size();
    }
    NODIS span<const double> band_freqs() const
    {
        return band_freqs_var;
    }
    // Power of each band after the most recent sample processed.
    NODIS span<const ProcFloat> band_powers() const
    {
        return band_powers_var;
    }

    static constexpr double k_max_band_freq_nyquist_ratio = 0.95;

private:
    static constexpr size_t k_max_sections = k_max_bp_filter_order / 2;

    // Analog prototype section, normalized to band frequency = 1 rad/s.
    struct AnalogSection {
        double w = NAN;  // Natural frequency.
        double R2 = NAN; // 1 / Q of the section.
    };
    struct Section {
        ProcFloat g = NAN, R2_plus_g = NAN, h = NAN;
        ProcFloat s1 = 0, s2 = 0;
    };
    struct Band {
        ProcFloat power_gain = NAN; // Squared normalizing gain of the bandpass.
        ProcFloat lpf_G = NAN;
        ProcFloat lpf_s = 0;
        array<Section, k_max_sections> sections;
    };

    double lpf_freq_ratio = NAN;
    double max_lpf_freq_hz = NAN;
    size_t num_sections = 0;
    array<AnalogSection, k_max_sections> analog_sections;
    double analog_power_gain = NAN;
    vector<double> band_freqs_var;
    vector<ProcFloat> band_powers_var;
    vector<Band> bands;
    size_t num_active_bands = 0; // The active bands are bands[0, num_active_bands).
    size_t lanes_override = 0;
    double sample_rate = NAN;

    template<size_t NS>
    void process_block_with_num_sections(std::span<const IOFloat> samples);
    // Processes bands[first_band, first_band + Lanes), the ones beyond num_active_bands are zero dummies.
    template<size_t NS, size_t Lanes>
    void process_bands(std::span<const IOFloat> samples, size_t first_band);
};
