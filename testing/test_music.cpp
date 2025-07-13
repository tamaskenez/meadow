#include "meadow/music.h"

#include <gtest/gtest.h>

TEST(music, midi2hz)
{
    EXPECT_DOUBLE_EQ(hzToMidi(440.0), 69.0);
    EXPECT_DOUBLE_EQ(hzToMidi(2 * 440.0), 69.0 + 12.0);
    EXPECT_DOUBLE_EQ(hzToMidi(440.0 / pow(2.0, (9.0 / 12.0))), 60.0);
}
