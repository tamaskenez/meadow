#include "meadow/sdl.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>

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

void push_sdl_user_event_any(std::any* event)
{
    SDL_Event sdl_event;
    SDL_zero(sdl_event);
    sdl_event.user.type = get_user_event_type_any();
    sdl_event.user.timestamp = SDL_GetTicksNS();
    sdl_event.user.data1 = event;
    // Note: false result from SDL_PushEvent might inidicate both error or that the event has been filtered out. Not
    // clear how to tell them apart, so we ignore the possible error.
    SDL_PushEvent(&sdl_event);
}
} // namespace detail

uint32_t get_user_event_type_any()
{
    static uint32_t s_user_event_type_any = CHECK_SDL(SDL_RegisterEvents(1));
    return s_user_event_type_any;
}

const SDL_UserEvent* sdl_user_event_cast(const SDL_Event* event, uint32_t user_event_type)
{
    return event->type == user_event_type ? reinterpret_cast<const SDL_UserEvent*>(event) : nullptr;
}

#if MEADOW_HAS_SDL_MIXER == 1
// Return the file extensions (without the dot) which the currently enabled MIX decoders support.
// Call MIX_Init() before calling this function.
vector<string> get_extensions_for_mix_decoders()
{
    vector<string> result;

    const static unordered_map<string, vector<string>> k_extensions_for_decoders = {
      {"WAV",       {"wav"} },
      {"STBVORBIS", {"ogg"} },
      {"DRFLAC",    {"flac"}},
      {"VOC",       {"voc"} },
      {"AIFF",      {"aiff"}},
      {"AU",        {"au"}  },
      {"DRMP3",     {"mp3"} }
    };

    for (int i : vi::iota(0, CHECK_SDL(MIX_GetNumAudioDecoders()))) {
        auto it = k_extensions_for_decoders.find(CHECK_SDL(MIX_GetAudioDecoder(i)));
        if (it != k_extensions_for_decoders.end()) {
            result.append_range(it->second);
        }
    }

    return result;
}
#endif
