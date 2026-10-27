#pragma once

#include <charconv>
#include <expected>
#include <string>
#include <string_view>

// Call std::from_chars with the argument.
// Return std::argument_out_of_domain if the string couldn't be fully parsed or whatever result/error from_chars
// returned.
template<class T>
std::expected<T, std::errc> expected_from_all_chars(std::string_view s)
{
    T value{};
    auto result = std::from_chars(s.data(), s.data() + s.size(), value);
    if (result.ec != std::errc()) {
        return std::unexpected(result.ec);
    }
    if (result.ptr != s.data() + s.size()) {
        return std::unexpected(std::errc::argument_out_of_domain);
    }
    return value;
}

std::string toupper(std::string_view s);
