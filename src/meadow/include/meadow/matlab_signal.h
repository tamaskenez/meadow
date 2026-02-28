#pragma once

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

struct ButterworthCoeffs {
    std::vector<double> b, a; // Direct-form IIR coefficients, a[0] = 1.
};

// Returns direct-form IIR coefficients [b, a] of a digital Butterworth filter.
// Frequencies are normalized: 0 < Wn < 1, where 1 = Nyquist frequency.
// LP and HP produce a filter of the given order.
// BP and BS apply the LP-to-BP/BS transformation, producing a filter of order 2*order.
ButterworthCoeffs butter(int order, const FilterType::V& filter);

} // namespace matlab
