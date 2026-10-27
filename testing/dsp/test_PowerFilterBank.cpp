#include "meadow/dsp/PowerFilterBank.h"
#include "meadow/math.h"
#include "meadow/matlab.h"
#include "meadow/matlab_signal.h"

#include <gtest/gtest.h>

#include <chrono>
#include <random>

namespace
{
constexpr double k_sample_rate = 48000;
constexpr size_t k_block_size = 512;

// Feeds a sine to the filter bank for `duration_sec` and returns the band powers averaged over the last quarter.
template<class IOFloat, class ProcFloat>
vector<double>
measure_sine(PowerFilterBank<IOFloat, ProcFloat>& pfb, double freq_hz, double amplitude, double duration_sec = 1.0)
{
    pfb.reset();
    const auto num_blocks = iround<size_t>(duration_sec * k_sample_rate / ifcast<double>(k_block_size));
    const auto first_measured_block = num_blocks * 3 / 4;
    vector<IOFloat> block(k_block_size);
    vector<double> sum(pfb.num_bands(), 0.0);
    size_t n = 0;
    for (size_t bi = 0; bi < num_blocks; ++bi) {
        for (auto& x : block) {
            x = static_cast<IOFloat>(amplitude * std::sin(2 * num::pi * freq_hz * ifcast<double>(n++) / k_sample_rate));
        }
        pfb.process_block(block);
        if (bi >= first_measured_block) {
            for (size_t k = 0; k < sum.size(); ++k) {
                sum[k] += pfb.band_powers()[k];
            }
        }
    }
    for (auto& s : sum) {
        s /= ifcast<double>(num_blocks - first_measured_block);
    }
    return sum;
}

double power_db(double power)
{
    return matlab::pow2db(power);
}
} // namespace

TEST(PowerFilterBank, band_freqs)
{
    const PowerFilterBank<double> pfb(20, 20000, 3, 4, 4.32, 10, 50);
    ASSERT_EQ(pfb.num_bands(), 30);
    EXPECT_DOUBLE_EQ(pfb.band_freqs().front(), 20);
    EXPECT_NEAR(pfb.band_freqs().back(), 20 * std::exp2(29.0 / 3), 1e-9);

    const PowerFilterBank<double> pfb2(125, 1000, 1, 4, 1.41, 10, 50);
    ASSERT_EQ(pfb2.num_bands(), 4);
    EXPECT_NEAR(pfb2.band_freqs().back(), 1000, 1e-9);
}

TEST(PowerFilterBank, Q_for_bands_per_octave)
{
    // One octave: edges at f0 / sqrt(2) and f0 * sqrt(2), bandwidth f0 / sqrt(2).
    EXPECT_NEAR(PowerFilterBank<double>::Q_for_bands_per_octave(1), std::sqrt(2.0), 1e-12);
    EXPECT_NEAR(PowerFilterBank<double>::Q_for_bands_per_octave(3), 4.3185, 1e-4);
}

TEST(PowerFilterBank, response_at_center_and_band_edges)
{
    constexpr double f0 = 1000;
    constexpr double A = 0.5;
    const double sine_power = A * A / 2;
    const double Q = PowerFilterBank<double>::Q_for_bands_per_octave(3);
    // -3 dB edges of the analog bandpass. Prewarping at 1 kHz / 48 kHz barely changes them.
    const double f_lo = f0 * (std::sqrt(1 + 1 / (4 * Q * Q)) - 1 / (2 * Q));
    const double f_hi = f0 * (std::sqrt(1 + 1 / (4 * Q * Q)) + 1 / (2 * Q));
    for (int order = 2; order <= PowerFilterBank<double>::k_max_bp_filter_order; order += 2) {
        SCOPED_TRACE(format("order = {}", order));
        PowerFilterBank<double> pfb(f0, f0, 1, order, Q, 100, 1000);
        pfb.prepare_to_play(k_sample_rate);
        EXPECT_NEAR(power_db(measure_sine(pfb, f0, A)[0]), power_db(sine_power), 0.05);
        EXPECT_NEAR(power_db(measure_sine(pfb, f_lo, A)[0]), power_db(sine_power) - 3.0103, 0.1);
        EXPECT_NEAR(power_db(measure_sine(pfb, f_hi, A)[0]), power_db(sine_power) - 3.0103, 0.1);
    }
}

TEST(PowerFilterBank, higher_order_is_more_selective)
{
    constexpr double f0 = 1000;
    const double Q = PowerFilterBank<double>::Q_for_bands_per_octave(3);
    double prev_lo = 0, prev_hi = 0;
    for (int order = 2; order <= PowerFilterBank<double>::k_max_bp_filter_order; order += 2) {
        SCOPED_TRACE(format("order = {}", order));
        PowerFilterBank<double> pfb(f0, f0, 1, order, Q, 100, 1000);
        pfb.prepare_to_play(k_sample_rate);
        const auto lo = power_db(measure_sine(pfb, f0 / 2, 1.0)[0]);
        const auto hi = power_db(measure_sine(pfb, f0 * 2, 1.0)[0]);
        EXPECT_LT(lo, -15);
        EXPECT_LT(hi, -15);
        if (order > 2) {
            EXPECT_LT(lo, prev_lo - 10);
            EXPECT_LT(hi, prev_hi - 10);
        }
        prev_lo = lo;
        prev_hi = hi;
    }
}

template<class ProcFloat>
void test_sine_peaks_in_its_band()
{
    using PFB = PowerFilterBank<float, ProcFloat>;
    PFB pfb(20, 20000, 3, 6, PFB::Q_for_bands_per_octave(3), 10, 50);
    pfb.prepare_to_play(k_sample_rate);
    for (const size_t k : {3u, 15u, 28u}) {
        const auto f = pfb.band_freqs()[k];
        SCOPED_TRACE(format("f = {}", f));
        const auto powers = measure_sine(pfb, f, 0.5, 2.0);
        EXPECT_EQ(std::ranges::max_element(powers) - powers.begin(), k);
        EXPECT_NEAR(power_db(powers[k]), power_db(0.125), 0.2);
    }
}

TEST(PowerFilterBank, sine_peaks_in_its_band)
{
    test_sine_peaks_in_its_band<double>();
}

TEST(PowerFilterBank, sine_peaks_in_its_band_float_processing)
{
    test_sine_peaks_in_its_band<float>();
}

TEST(PowerFilterBank, bands_near_nyquist_are_inactive)
{
    PowerFilterBank<double> pfb(1000, 32000, 1, 4, 1.41, 10, 10);
    pfb.prepare_to_play(k_sample_rate);
    ASSERT_EQ(pfb.num_bands(), 6);
    const auto powers = measure_sine(pfb, 16000, 0.5, 0.1);
    EXPECT_GT(powers[4], 0);   // 16 kHz
    EXPECT_EQ(powers[5], 0.0); // 32 kHz, above Nyquist
}

// The processing of each number of lanes must match the scalar processing (1 lane).
TEST(PowerFilterBank, lanes_match_scalar)
{
    using PFB = PowerFilterBank<double, double>;
    std::mt19937 rng(1);
    std::normal_distribution<double> dist(0, 0.3);
    vector<double> noise(4000);
    for (auto& x : noise) {
        x = dist(rng);
    }
    for (int order = 2; order <= PFB::k_max_bp_filter_order; order += 2) {
        // 30 bands: the last block is partially filled for every number of lanes > 1.
        PFB pfb(20, 20000, 3, order, PFB::Q_for_bands_per_octave(3), 10, 50);
        pfb.prepare_to_play(k_sample_rate);
        pfb.set_lanes(1);
        pfb.process_block(noise);
        const vector<double> expected(pfb.band_powers().begin(), pfb.band_powers().end());
        for (const auto lanes : PFB::k_supported_lanes) {
            SCOPED_TRACE(format("order = {}, lanes = {}", order, lanes));
            pfb.reset();
            pfb.set_lanes(lanes);
            pfb.process_block(noise);
            for (size_t k = 0; k < expected.size(); ++k) {
                EXPECT_NEAR(pfb.band_powers()[k], expected[k], 1e-12 * expected[k]) << format("band {}", k);
            }
        }
    }
}

// Prints the processing time per band and sample for each number of sections and lanes. Run in a release build.
TEST(PowerFilterBank, DISABLED_benchmark_lanes)
{
    using PFB = PowerFilterBank<float, float>;
    constexpr double bands_per_octave = 12;
    constexpr size_t block_size = 1600;
    constexpr size_t num_blocks = 300;
    std::mt19937 rng(1);
    std::normal_distribution<float> dist(0, 0.3f);
    vector<float> noise(block_size);
    for (auto& x : noise) {
        x = dist(rng);
    }
    println("ns / (band * sample), {} bands/octave, 20 Hz - 20 kHz, {} Hz", bands_per_octave, k_sample_rate);
    print("{:>8}", "lanes");
    for (const auto lanes : PFB::k_supported_lanes) {
        print("{:>8}", lanes);
    }
    println("");
    for (int order = 2; order <= PFB::k_max_bp_filter_order; order += 2) {
        PFB pfb(20, 20000, bands_per_octave, order, PFB::Q_for_bands_per_octave(bands_per_octave), 10, 50);
        pfb.prepare_to_play(k_sample_rate);
        print("{:>8}", format("NS={}", order / 2));
        for (const auto lanes : PFB::k_supported_lanes) {
            pfb.reset();
            pfb.set_lanes(lanes);
            pfb.process_block(noise); // Warm-up.
            const auto t0 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < num_blocks; ++i) {
                pfb.process_block(noise);
            }
            const auto t1 = std::chrono::steady_clock::now();
            const auto ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
            print("{:>8.3f}", ns / ifcast<double>(num_blocks * block_size * pfb.num_bands()));
        }
        println("");
    }
}

TEST(PowerFilterBank, DISABLED_playground_envelope)
{
    constexpr double k_band_freq_hz = 1140.0;
    constexpr double k_sine_freq_hz = 1140.0;
    const double k_amplitude = sqrt(2.0);
    const auto Q = PowerFilterBank<double>::Q_for_bands_per_octave(3.0);

    PowerFilterBank<double> pfb(k_band_freq_hz, k_band_freq_hz, 1, 4, Q, 5, 100);
    pfb.prepare_to_play(k_sample_rate);
    constexpr double duration_sec = 0.2;
    const auto max_signal_n = iround<size_t>(duration_sec / 2 * k_sample_rate);
    const auto block_size = iround<size_t>(k_sample_rate / 1000.0);
    const auto num_blocks = iround<size_t>(duration_sec * k_sample_rate / ifcast<double>(block_size));
    vector<double> block(block_size);
    vector<double> power_seq;
    power_seq.reserve(num_blocks);
    size_t n = 0;
    for (size_t bi = 0; bi < num_blocks; ++bi) {
        for (auto& x : block) {
            if (n < max_signal_n) {
                x = k_amplitude * std::sin(2 * num::pi * k_sine_freq_hz * ifcast<double>(n++) / k_sample_rate);
            } else {
                x = 0;
            }
        }
        pfb.process_block(block);
        power_seq.push_back(pfb.band_powers()[0]);
    }

    matlab::saveAscii("/tmp/pf.m", span(power_seq)).value();
}
