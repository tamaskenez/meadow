#pragma once

#include <cassert>
#include <span>
#include <variant>
#include <vector>

namespace matlab
{

namespace FilterType
{
struct LowPass {
    double cutoff;
};
struct HighPass {
    double cutoff;
};
struct BandPass {
    double low_cutoff, high_cutoff;
};
struct BandStop {
    double low_cutoff, high_cutoff;
};
using V = std::variant<LowPass, HighPass, BandPass, BandStop>;
} // namespace FilterType

struct TransferFunctionCoeffs {
    std::vector<double> b, a; // Direct-form IIR coefficients, a[0] = 1.

    template<size_t N>
    std::span<const double, N> b_as_static_span() const
    {
        assert(b.size() == N);
        return std::span<const double, N>(b.data(), b.size());
    }
    template<size_t N>
    std::span<const double, N> a_as_static_span() const
    {
        assert(a.size() == N);
        return std::span<const double, N>(a.data(), a.size());
    }
};

// Returns direct-form IIR coefficients [b, a] of a digital Butterworth filter.
// Frequencies are normalized: 0 < Wn < 1, where 1 = Nyquist frequency.
// LP and HP produce a filter of the given order.
// BP and BS apply the LP-to-BP/BS transformation, producing a filter of order 2*order.
TransferFunctionCoeffs butter(int order, const FilterType::V& filter);

// Return the frequency response of the specified digital filter at the normalized frequency `w`.
// NOTE: following MATLAB conventions, `w` is in rad/s (positive frequencies: 0..pi),
// unlike other functions (e.g. `butter`) where the frequency is
// normalized to Nyquist (positive frequencies: 0..1)
std::complex<double> freqz(std::span<const double> b, std::span<const double> a, double w);
} // namespace matlab
