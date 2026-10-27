#include "meadow/string.h"

#include <gtest/gtest.h>

TEST(string, toupper)
{
    EXPECT_EQ(toupper(""), "");
    EXPECT_EQ(toupper("abc"), "ABC");
    EXPECT_EQ(toupper("AbC"), "ABC");
    EXPECT_EQ(toupper("c#4 - 1.5 Hz"), "C#4 - 1.5 HZ");
    EXPECT_EQ(toupper(std::string_view("abcdef").substr(1, 3)), "BCD");
}

TEST(string, expected_from_all_chars_int)
{
    EXPECT_EQ(expected_from_all_chars<int>("0"), 0);
    EXPECT_EQ(expected_from_all_chars<int>("42"), 42);
    EXPECT_EQ(expected_from_all_chars<int>("-17"), -17);
    EXPECT_EQ(expected_from_all_chars<int>(std::string_view("123456").substr(1, 3)), 234);

    EXPECT_EQ(expected_from_all_chars<int>(""), std::unexpected(std::errc::invalid_argument));
    EXPECT_EQ(expected_from_all_chars<int>("abc"), std::unexpected(std::errc::invalid_argument));
    EXPECT_EQ(expected_from_all_chars<int>(" 1"), std::unexpected(std::errc::invalid_argument));
    EXPECT_EQ(expected_from_all_chars<int>("+1"), std::unexpected(std::errc::invalid_argument));
    EXPECT_EQ(expected_from_all_chars<int>("99999999999999999999"), std::unexpected(std::errc::result_out_of_range));

    EXPECT_EQ(expected_from_all_chars<int>("12x"), std::unexpected(std::errc::argument_out_of_domain));
    EXPECT_EQ(expected_from_all_chars<int>("1 "), std::unexpected(std::errc::argument_out_of_domain));
    EXPECT_EQ(expected_from_all_chars<int>("1.5"), std::unexpected(std::errc::argument_out_of_domain));
}

TEST(string, expected_from_all_chars_other_types)
{
    static_assert(std::is_same_v<decltype(expected_from_all_chars<double>("")), std::expected<double, std::errc>>);
    static_assert(std::is_same_v<decltype(expected_from_all_chars<int64_t>("")), std::expected<int64_t, std::errc>>);

    EXPECT_EQ(expected_from_all_chars<double>("1.5"), 1.5);
    EXPECT_EQ(expected_from_all_chars<double>("-2.25e2"), -225.0);
    EXPECT_EQ(expected_from_all_chars<int64_t>("10000000000"), int64_t(10000000000));
    EXPECT_EQ(expected_from_all_chars<uint8_t>("255"), uint8_t(255));
    EXPECT_EQ(expected_from_all_chars<uint8_t>("256"), std::unexpected(std::errc::result_out_of_range));
    EXPECT_EQ(expected_from_all_chars<uint8_t>("-1"), std::unexpected(std::errc::invalid_argument));
}
