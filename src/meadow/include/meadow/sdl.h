#pragma once

#pragma once

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#if MEADOW_HAS_SDL_MIXER == 1
  #include <SDL3_mixer/SDL_mixer.h>
#endif

#include <any>
#include <chrono>
#include <memory>
#include <source_location>
#include <string>
#include <type_traits>
#include <vector>

// A std::unique_ptr-like object for SDL types, like SDL_Window, SDL_Renderer, etc.. which calls the appropriate
// SDL_Destroy* function in its destructor.
template<class T>
class sdl_unique_ptr;

template<class T>
sdl_unique_ptr(T*) -> sdl_unique_ptr<T>;

#define DEFINE_SDL_UNIQUE_PTR(TYPE, DELETER_FN)                               \
    struct TYPE##_Deleter {                                                   \
        void operator()(TYPE* p) const                                        \
        {                                                                     \
            DELETER_FN(p);                                                    \
        }                                                                     \
    };                                                                        \
    template<>                                                                \
    class sdl_unique_ptr<TYPE> : public std::unique_ptr<TYPE, TYPE##_Deleter> \
    {                                                                         \
    public:                                                                   \
        sdl_unique_ptr() = default;                                           \
        explicit sdl_unique_ptr(TYPE* p)                                      \
            : std::unique_ptr<TYPE, TYPE##_Deleter>(p)                        \
        {                                                                     \
        }                                                                     \
    };

DEFINE_SDL_UNIQUE_PTR(SDL_Window, SDL_DestroyWindow)
DEFINE_SDL_UNIQUE_PTR(SDL_Renderer, SDL_DestroyRenderer)
DEFINE_SDL_UNIQUE_PTR(SDL_Texture, SDL_DestroyTexture)
DEFINE_SDL_UNIQUE_PTR(SDL_Surface, SDL_DestroySurface)
DEFINE_SDL_UNIQUE_PTR(SDL_Palette, SDL_DestroyPalette)
DEFINE_SDL_UNIQUE_PTR(SDL_GPUDevice, SDL_DestroyGPUDevice)
DEFINE_SDL_UNIQUE_PTR(SDL_AudioStream, SDL_DestroyAudioStream)

#if MEADOW_HAS_SDL_MIXER == 1
DEFINE_SDL_UNIQUE_PTR(MIX_Mixer, MIX_DestroyMixer)
DEFINE_SDL_UNIQUE_PTR(MIX_Audio, MIX_DestroyAudio)
DEFINE_SDL_UNIQUE_PTR(MIX_AudioDecoder, MIX_DestroyAudioDecoder)
#endif

#undef DEFINE_SDL_UNIQUE_PTR

namespace detail
{
void check_sdl_handle_failure(bool terminate_on_error, const std::source_location& location);
}

template<class T>
T check_sdl(T result, bool terminate_on_error, const std::source_location location = std::source_location::current())
{
    bool success;
    if constexpr (std::is_same_v<T, bool>) {
        success = result;
    } else if constexpr (std::is_pointer_v<T>) {
        success = result != nullptr;
    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        success = result != T(0);
    } else {
        static_assert(
          false,
          "The result type of the SDL function is expected to "
          "be bool, pointer or integral."
        );
    }
    if (!success) {
        detail::check_sdl_handle_failure(terminate_on_error, location);
    }
    return result;
}

#define CHECK_SDL(X) check_sdl(X, true)
#define EXPECT_SDL(X) check_sdl(X, false)

template<class T>
const T* sdl_event_cast(const SDL_Event* event);

const SDL_UserEvent* sdl_user_event_cast(const SDL_Event* event, uint32_t user_event_type);

// Call SDL_PushEvent with SDL_UserEvent by moving `event` onto the heap and SDL_UserEvent::data1 contains the pointer
// to that.
namespace detail
{
void push_sdl_user_event_any(std::any* event); // Takes ownership.
}
template<class T>
    requires(!std::is_same_v<std::decay_t<T>, std::any*>)
void push_sdl_user_event_any(T&& payload)
{
    detail::push_sdl_user_event_any(new std::any(std::forward<T>(payload)));
}
uint32_t get_user_event_type_any();

#if MEADOW_HAS_SDL_MIXER == 1
// Return the file extensions (without the dot) which the currently enabled MIX decoders support.
// Call MIX_Init() before calling this function.
std::vector<std::string> get_extensions_for_mix_decoders();
#endif

class sdl_clock
{
public:
    using rep = int64_t;
    using period = std::nano;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<sdl_clock>;
    static constexpr bool is_steady()
    {
        return true;
    }
    static time_point now() noexcept
    {
        return time_point(duration(SDL_GetTicksNS()));
    }
    static time_point from_ticks_ns(uint64_t t)
    {
        return time_point(duration(t));
    }
};

std::string sdl_get_event_description(SDL_Event* event);

std::vector<std::string> sdl_get_render_drivers();
std::vector<std::string> sdl_enumerate_properties(SDL_PropertiesID props);
std::string sdl_get_property_as_string(SDL_PropertiesID props, const char* name);

struct SDLGPUDeviceInfo {
    std::string name;
    std::vector<std::string> shader_formats;
    std::vector<std::pair<std::string, std::string>> props;
};

struct SDLRendererInfo {
    std::string name;
    std::string vsync;
    std::vector<std::pair<std::string, std::string>> props;
    std::optional<SDLGPUDeviceInfo> gpu_device_info;
};

SDLRendererInfo get_sdl_renderer_info(SDL_Renderer* renderer);
