#pragma once

#include <cmath>

inline double hzToMidi(double hz)
{
    return 12.0 * log2(hz) + (69.0 - 12.0 * log2(440.0));
}
