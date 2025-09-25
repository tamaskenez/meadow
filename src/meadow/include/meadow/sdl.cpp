#include "sdl.h"

#include <SDL3/SDL_log.h>

namespace detail
{
void check_sdl_handle_failure(bool terminate_on_error, const std::source_location& location)
{
    SDL_LogMessage(
      SDL_LOG_CATEGORY_APPLICATION,
      terminate_on_error ? SDL_LOG_PRIORITY_CRITICAL : SDL_LOG_PRIORITY_ERROR,
      "%s",
      std::format("{}:{}:{} sdl_error: {}", location.file_name(), location.line(), location.column(), SDL_GetError())
        .c_str()
    );

#ifdef NDEBUG
    if (terminate_on_error) {
        std::terminate();
    }
#endif
    assert(false);
}
} // namespace detail
