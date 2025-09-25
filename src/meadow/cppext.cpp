#include "meadow/cppext.h"

#if MEADOW_HAS_ABSL == 0
[[noreturn]] void meadow_check_failed_handler(const char* condition, const std::source_location location)
{
    std::println(
      stderr, "{}:{}:{} CHECK failed: {}", location.file_name(), location.line(), location.column(), condition
    );
    std::terminate();
}
#endif
