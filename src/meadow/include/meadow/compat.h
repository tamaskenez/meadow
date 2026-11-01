#pragma once

// compat::format, compat::print, compat::println: the std:: versions if the standard library provides them for the
// current target (e.g. libc++ before macOS 13.3 doesn't), otherwise the fmt:: versions. MEADOW_HAS_STD_FORMAT_AND_PRINT
// is detected in CMake.
// Only the common subset of the two APIs is supported, e.g. custom formatters must be written for the selected one.
//
// In the fmt case they're function objects instead of functions: if unqualified lookup finds an object, ADL is not
// performed. Otherwise an unqualified `format("{}", std_string)` (with `using compat::format`, see cppext.h) would also
// find std::format via ADL, as <format> is included transitively by other std headers (e.g. <chrono>, <ostream>), and
// std::format would be selected.

#if MEADOW_HAS_STD_FORMAT_AND_PRINT
  #include <format>
  #include <print>
#else
  #include <fmt/format.h>

  #include <cstdio>
  #include <string>
  #include <utility>
#endif

namespace compat
{
#if MEADOW_HAS_STD_FORMAT_AND_PRINT
using std::format;
using std::format_string;
using std::print;
using std::println;
#else
using fmt::format_string;

namespace detail
{
struct Format {
    template<class... Args>
    std::string operator()(format_string<Args...> f, Args&&... args) const
    {
        return fmt::format(f, std::forward<Args>(args)...);
    }
};

struct Print {
    template<class... Args>
    void operator()(format_string<Args...> f, Args&&... args) const
    {
        fmt::print(f, std::forward<Args>(args)...);
    }
    template<class... Args>
    void operator()(FILE* stream, format_string<Args...> f, Args&&... args) const
    {
        fmt::print(stream, f, std::forward<Args>(args)...);
    }
};

struct Println {
    void operator()() const
    {
        fmt::println("");
    }
    void operator()(FILE* stream) const
    {
        fmt::println(stream, "");
    }
    template<class... Args>
    void operator()(format_string<Args...> f, Args&&... args) const
    {
        fmt::println(f, std::forward<Args>(args)...);
    }
    template<class... Args>
    void operator()(FILE* stream, format_string<Args...> f, Args&&... args) const
    {
        fmt::println(stream, f, std::forward<Args>(args)...);
    }
};
} // namespace detail

inline constexpr detail::Format format;
inline constexpr detail::Print print;
inline constexpr detail::Println println;
#endif
} // namespace compat
