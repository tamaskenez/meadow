#pragma once

// compat::format, compat::print, compat::println: the std:: versions if the standard library provides them for the
// current target (e.g. libc++ before macOS 13.3 doesn't), otherwise the fmt:: versions. MEADOW_HAS_STD_FORMAT_AND_PRINT
// is detected in CMake.
// Only the common subset of the two APIs is supported, e.g. custom formatters must be written for the selected one.

#if MEADOW_HAS_STD_FORMAT_AND_PRINT
  #include <format>
  #include <print>
#else
  #include <fmt/format.h>
#endif

namespace compat
{
#if MEADOW_HAS_STD_FORMAT_AND_PRINT
using std::format;
using std::format_string;
using std::print;
using std::println;
#else
using fmt::format;
using fmt::format_string;
using fmt::print;
using fmt::println;
#endif
} // namespace compat
