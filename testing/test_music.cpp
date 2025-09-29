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
