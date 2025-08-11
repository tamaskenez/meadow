#include "meadow/music.h"

#include <gtest/gtest.h>

TEST(music, hzToMidi)
{
    EXPECT_DOUBLE_EQ(hzToMidi(440.0), 69.0);
    EXPECT_DOUBLE_EQ(hzToMidi(2 * 440.0), 69.0 + 12.0);
    EXPECT_DOUBLE_EQ(hzToMidi(440.0 / pow(2.0, (9.0 / 12.0))), 60.0);
}

TEST(music, ratioToSemitones)
{
    EXPECT_DOUBLE_EQ(ratioToSemitones(0.5), -12.0);
    EXPECT_DOUBLE_EQ(ratioToSemitones(1.0), 0.0);
    EXPECT_DOUBLE_EQ(ratioToSemitones(pow(2.0, 1.0 / 12.0)), 1.0);
    EXPECT_DOUBLE_EQ(ratioToSemitones(2.0), 12.0);
}
