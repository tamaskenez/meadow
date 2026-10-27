#include "meadow/music.h"

#include <charconv>
#include <optional>

std::optional<int> musical_note_letter_to_semitones_from_c(char note, bool german)
{
    switch (tolower(note)) {
    case 'c':
        return 0;
    case 'd':
        return 2;
    case 'e':
        return 4;
    case 'f':
        return 5;
    case 'g':
        return 7;
    case 'a':
        return 9;
    case 'h':
        return german ? std::optional(11) : nullopt;
    case 'b':
        return german ? 10 : 11;
    default:
        return std::nullopt;
    }
}

// Parses scientific pitch notation like "a4", "C#3", "eb2", "c-1" (case-insensitive). C4 = 60.
std::optional<int> midi_note_name_to_number(std::string_view note)
{
    if (note.empty()) {
        return std::nullopt;
    }
    auto semitones = musical_note_letter_to_semitones_from_c(note[0], false);
    if (!semitones) {
        return std::nullopt;
    }
    note.remove_prefix(1);
    int s = *semitones;
    for (; !note.empty() && (note[0] == '#' || note[0] == 'b'); note.remove_prefix(1)) {
        s += note[0] == '#' ? 1 : -1;
    }
    int octave = 0;
    const auto [ptr, ec] = std::from_chars(note.data(), note.data() + note.size(), octave);
    if (ec != std::errc() || ptr != note.data() + note.size()) {
        return std::nullopt;
    }
    return 12 * (octave + 1) + s;
}
