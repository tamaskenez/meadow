#pragma once

#include <cmath>
#include <concepts>

template<class T>
    requires std::is_floating_point_v<T>
T ratio2semitones(T ratio)
{
    return 12 * std::log2(ratio);
}

template<class T>
    requires std::is_floating_point_v<T>
T semitones2ratio(T semitones)
{
    return std::exp2(semitones / 12);
}

template<class T>
    requires std::is_floating_point_v<T>
T hz2midi(T hz)
{
    return 69 + ratio2semitones(hz / 440);
}

template<class T>
    requires std::is_floating_point_v<T>
T midi2hz(T midi)
{
    return 440 * semitones2ratio(midi - 69);
}
