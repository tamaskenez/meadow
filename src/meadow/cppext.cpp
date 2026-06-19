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

expected<FILE*, errno_t> try_fopen(const char* filename, const char* mode)
{
#ifdef _MSC_VER
    FILE* f{};
    if (errno_t e = fopen_s(&f, filename, mode); e != 0) {
        return unexpected(e);
    }
    return f;
#else
    if (FILE* f = fopen(filename, mode)) {
        return f;
    }
    return unexpected(errno);
#endif
}
