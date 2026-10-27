#pragma once

#include <cmath>
#include <concepts>
#include <optional>
#include <string_view>

template<class T>
double ratio2semitones(T ratio)
{
    return 12 * std::log2(static_cast<double>(ratio));
}

template<class T>
double semitones2ratio(T semitones)
{
    return std::exp2(semitones / 12.0);
}

template<class T>
double hz2midi(T hz)
{
    return 69 + ratio2semitones(hz / 440.0);
}

template<class T>
double midi2hz(T midi)
{
    return 440 * semitones2ratio(midi - 69.0);
}

std::optional<int> musical_note_letter_to_semitones_from_c(char note, bool german);
std::optional<int> midi_note_name_to_number(std::string_view note);
