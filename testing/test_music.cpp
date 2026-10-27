#include "meadow/music.h"

#include <gtest/gtest.h>

TEST(music, hz2midi)
{
    EXPECT_DOUBLE_EQ(hz2midi(440.0), 69.0);
    EXPECT_DOUBLE_EQ(hz2midi(2 * 440.0), 69.0 + 12.0);
    EXPECT_DOUBLE_EQ(hz2midi(440.0 / pow(2.0, (9.0 / 12.0))), 60.0);
}

TEST(music, midi2hz)
{
    EXPECT_DOUBLE_EQ(440.0, midi2hz(69.0));
    EXPECT_DOUBLE_EQ(2 * 440.0, midi2hz(69.0 + 12.0));
    EXPECT_DOUBLE_EQ(440.0 / pow(2.0, (9.0 / 12.0)), midi2hz(60.0));
}

TEST(music, ratio2semitones)
{
    EXPECT_DOUBLE_EQ(ratio2semitones(0.5), -12.0);
    EXPECT_DOUBLE_EQ(ratio2semitones(1.0), 0.0);
    EXPECT_DOUBLE_EQ(ratio2semitones(pow(2.0, 1.0 / 12.0)), 1.0);
    EXPECT_DOUBLE_EQ(ratio2semitones(2.0), 12.0);
}

TEST(music, semitones2ratio)
{
    EXPECT_DOUBLE_EQ(0.5, semitones2ratio(-12.0));
    EXPECT_DOUBLE_EQ(1.0, semitones2ratio(0.0));
    EXPECT_DOUBLE_EQ(pow(2.0, 1.0 / 12.0), semitones2ratio(1.0));
    EXPECT_DOUBLE_EQ(2.0, semitones2ratio(12.0));
}

TEST(music, musical_note_letter_to_semitones_from_c)
{
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('c', false), 0);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('d', false), 2);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('e', false), 4);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('f', false), 5);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('g', false), 7);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('a', false), 9);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('b', false), 11);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('h', false), std::nullopt);

    EXPECT_EQ(musical_note_letter_to_semitones_from_c('b', true), 10);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('h', true), 11);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('a', true), 9);

    EXPECT_EQ(musical_note_letter_to_semitones_from_c('C', false), 0);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('B', false), 11);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('H', true), 11);

    EXPECT_EQ(musical_note_letter_to_semitones_from_c('x', false), std::nullopt);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('#', true), std::nullopt);
    EXPECT_EQ(musical_note_letter_to_semitones_from_c('\0', false), std::nullopt);
}

TEST(music, midi_note_name_to_number)
{
    EXPECT_EQ(midi_note_name_to_number("a4"), 69);
    EXPECT_EQ(midi_note_name_to_number("c4"), 60);
    EXPECT_EQ(midi_note_name_to_number("b3"), 59);
    EXPECT_EQ(midi_note_name_to_number("a0"), 21);
    EXPECT_EQ(midi_note_name_to_number("c8"), 108);
    EXPECT_EQ(midi_note_name_to_number("c10"), 132);
    EXPECT_EQ(midi_note_name_to_number("c-1"), 0);
    EXPECT_EQ(midi_note_name_to_number("A4"), 69);

    EXPECT_EQ(midi_note_name_to_number("c#4"), 61);
    EXPECT_EQ(midi_note_name_to_number("C#4"), 61);
    EXPECT_EQ(midi_note_name_to_number("eb2"), 39);
    EXPECT_EQ(midi_note_name_to_number("bb3"), 58);
    EXPECT_EQ(midi_note_name_to_number("f##4"), 67);
    EXPECT_EQ(midi_note_name_to_number("cb4"), 59);
    EXPECT_EQ(midi_note_name_to_number("b#3"), 60);

    EXPECT_EQ(midi_note_name_to_number(""), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("c"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("c#"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("x4"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("h4"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("c4x"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("c 4"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("c+4"), std::nullopt);
    EXPECT_EQ(midi_note_name_to_number("4c"), std::nullopt);

    // Consistent with midi2hz.
    EXPECT_DOUBLE_EQ(midi2hz(*midi_note_name_to_number("a4")), 440.0);
    EXPECT_DOUBLE_EQ(midi2hz(*midi_note_name_to_number("a5")), 880.0);
}
