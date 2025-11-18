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

#define DEFINE_SDL_EVENT_CAST_1(EVENT_TYPE, TYPE_1)                                          \
    template<>                                                                               \
    const EVENT_TYPE* sdl_event_cast(const SDL_Event* event)                                 \
    {                                                                                        \
        return event->type == TYPE_1 ? reinterpret_cast<const EVENT_TYPE*>(event) : nullptr; \
    }

#define DEFINE_SDL_EVENT_CAST_2(EVENT_TYPE, TYPE_1, TYPE_2)                                                           \
    template<>                                                                                                        \
    const EVENT_TYPE* sdl_event_cast(const SDL_Event* event)                                                          \
    {                                                                                                                 \
        return event->type == TYPE_1 || event->type == TYPE_2 ? reinterpret_cast<const EVENT_TYPE*>(event) : nullptr; \
    }

#define DEFINE_SDL_EVENT_CAST_3(EVENT_TYPE, TYPE_1, TYPE_2, TYPE_3)                    \
    template<>                                                                         \
    const EVENT_TYPE* sdl_event_cast(const SDL_Event* event)                           \
    {                                                                                  \
        return event->type == TYPE_1 || event->type == TYPE_2 || event->type == TYPE_3 \
               ? reinterpret_cast<const EVENT_TYPE*>(event)                            \
               : nullptr;                                                              \
    }

#define DEFINE_SDL_EVENT_CAST_FROM_TO(EVENT_TYPE, TYPE_FROM, TYPE_TO)                                          \
    template<>                                                                                                 \
    const EVENT_TYPE* sdl_event_cast(const SDL_Event* event)                                                   \
    {                                                                                                          \
        return TYPE_FROM <= event->type && event->type <= TYPE_TO ? reinterpret_cast<const EVENT_TYPE*>(event) \
                                                                  : nullptr;                                   \
    }

DEFINE_SDL_EVENT_CAST_1(SDL_QuitEvent, SDL_EVENT_QUIT)
DEFINE_SDL_EVENT_CAST_1(SDL_MouseMotionEvent, SDL_EVENT_MOUSE_MOTION)
DEFINE_SDL_EVENT_CAST_2(SDL_KeyboardEvent, SDL_EVENT_KEY_DOWN, SDL_EVENT_KEY_UP)
DEFINE_SDL_EVENT_CAST_2(SDL_MouseButtonEvent, SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_EVENT_MOUSE_BUTTON_UP)
DEFINE_SDL_EVENT_CAST_FROM_TO(SDL_TouchFingerEvent, SDL_EVENT_FINGER_DOWN, SDL_EVENT_FINGER_CANCELED)
DEFINE_SDL_EVENT_CAST_FROM_TO(SDL_WindowEvent, SDL_EVENT_WINDOW_FIRST, SDL_EVENT_WINDOW_LAST)

std::string sdl_get_event_description(SDL_Event* event)
{
    int n = SDL_GetEventDescription(event, nullptr, 0);
    std::string s(iicast<size_t>(n) + 1, 0);
    SDL_GetEventDescription(event, s.data(), n);
    while (!s.empty() && s.back() == 0) {
        s.pop_back();
    }
    return s;
}

vector<string> sdl_get_render_drivers()
{
    vector<string> r;
    for (int i : vi_iota<int>(0, SDL_GetNumRenderDrivers())) {
        const char* name = SDL_GetRenderDriver(i);
        CHECK(name);
        r.push_back(name);
    }
    return r;
}

namespace
{
void sdl_enumerate_properties_callback(void* userdata, SDL_PropertiesID /*props*/, const char* name)
{
    auto* vs = reinterpret_cast<vector<string>*>(userdata);
    vs->push_back(name);
}
} // namespace

vector<string> sdl_enumerate_properties(SDL_PropertiesID props)
{
    vector<string> vs;
    CHECK_SDL(SDL_EnumerateProperties(props, sdl_enumerate_properties_callback, &vs));
    return vs;
}

string sdl_get_property_as_string(SDL_PropertiesID props, const char* name)
{
    SDL_PropertyType pt = SDL_GetPropertyType(props, name);
    switch (pt) {
    case SDL_PROPERTY_TYPE_INVALID:
        LOG(FATAL) << format("Invalid property: {}", name);
    case SDL_PROPERTY_TYPE_POINTER:
        return format("{}", SDL_GetPointerProperty(props, name, nullptr));
    case SDL_PROPERTY_TYPE_STRING:
        return SDL_GetStringProperty(props, name, "<ERROR>");
    case SDL_PROPERTY_TYPE_NUMBER:
        return std::to_string(SDL_GetNumberProperty(props, name, INT64_MIN));
    case SDL_PROPERTY_TYPE_FLOAT:
        return std::to_string(SDL_GetFloatProperty(props, name, NAN));
    case SDL_PROPERTY_TYPE_BOOLEAN:
        return SDL_GetBooleanProperty(props, name, false) ? "true" : "false";
    }
}

namespace
{
const vector<pair<uint32_t, const char*>> k_sdl_gpu_shader_formats = {
#define SHADER_FORMAT_ITEM(X) {X, #X}
  SHADER_FORMAT_ITEM(SDL_GPU_SHADERFORMAT_PRIVATE),
  SHADER_FORMAT_ITEM(SDL_GPU_SHADERFORMAT_SPIRV),
  SHADER_FORMAT_ITEM(SDL_GPU_SHADERFORMAT_DXBC),
  SHADER_FORMAT_ITEM(SDL_GPU_SHADERFORMAT_DXIL),
  SHADER_FORMAT_ITEM(SDL_GPU_SHADERFORMAT_MSL),
  SHADER_FORMAT_ITEM(SDL_GPU_SHADERFORMAT_METALLIB)
#undef SHADER_FORMAT_ITEM
};

const vector<pair<SDL_Colorspace, const char*>> k_sdl_colorspace = {
#define COLORSPACE_ITEM(X) {X, #X}
  COLORSPACE_ITEM(SDL_COLORSPACE_SRGB),
  COLORSPACE_ITEM(SDL_COLORSPACE_SRGB_LINEAR),
  COLORSPACE_ITEM(SDL_COLORSPACE_HDR10),
  COLORSPACE_ITEM(SDL_COLORSPACE_JPEG),
  COLORSPACE_ITEM(SDL_COLORSPACE_BT601_LIMITED),
  COLORSPACE_ITEM(SDL_COLORSPACE_BT601_FULL),
  COLORSPACE_ITEM(SDL_COLORSPACE_BT709_LIMITED),
  COLORSPACE_ITEM(SDL_COLORSPACE_BT709_FULL),
  COLORSPACE_ITEM(SDL_COLORSPACE_BT2020_LIMITED),
  COLORSPACE_ITEM(SDL_COLORSPACE_BT2020_FULL)
#undef COLORSPACE_ITEM
};
} // namespace

SDLRendererInfo get_sdl_renderer_info(SDL_Renderer* renderer)
{
    SDLRendererInfo ri;
    if (auto* name = SDL_GetRendererName(renderer)) {
        ri.name = name;
    } else {
        ri.name = format("<ERROR: {}>", SDL_GetError());
    }
    int vsync;
    if (SDL_GetRenderVSync(renderer, &vsync)) {
        ri.vsync = std::to_string(vsync);
    } else {
        ri.vsync = format("<ERROR: {}>", SDL_GetError());
    }
    {
        SDL_PropertiesID props_id = CHECK_SDL(SDL_GetRendererProperties(renderer));
        auto prop_names = sdl_enumerate_properties(props_id);
        for (auto& pn : prop_names) {
            bool found = false;
            if (pn == SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER) {
                auto c = SDL_GetNumberProperty(props_id, pn.c_str(), 0);
                for (auto [k, v] : k_sdl_colorspace) {
                    if (k == c) {
                        ri.props.push_back(pair(pn, v));
                        found = true;
                        break;
                    }
                }
            } else if (pn == SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER) {
                const SDL_PixelFormat* pixel_formats =
                  static_cast<const SDL_PixelFormat*>(SDL_GetPointerProperty(props_id, pn.c_str(), nullptr));
                if (pixel_formats) {
                    found = true;
                    string s;
                    for (; *pixel_formats != SDL_PIXELFORMAT_UNKNOWN; ++pixel_formats) {
                        if (!s.empty()) {
                            s += '|';
                        }
                        s += SDL_GetPixelFormatName(*pixel_formats);
                    }
                    ri.props.push_back(pair(pn, s));
                }
            }
            if (!found) {
                ri.props.push_back(pair(pn, sdl_get_property_as_string(props_id, pn.c_str())));
            }
        }
    }
    if (SDL_GPUDevice* gpu_device = SDL_GetGPURendererDevice(renderer)) {
        SDLGPUDeviceInfo gdi;
        gdi.name = CHECK_SDL(SDL_GetGPUDeviceDriver(gpu_device));
        SDL_GPUShaderFormat gsf = SDL_GetGPUShaderFormats(gpu_device);
        for (auto [v, s] : k_sdl_gpu_shader_formats) {
            if (gsf & v) {
                gdi.shader_formats.push_back(s);
            }
        }
        {
            SDL_PropertiesID props_id = CHECK_SDL(SDL_GetGPUDeviceProperties(gpu_device));
            auto prop_names = sdl_enumerate_properties(props_id);
            for (auto& pn : prop_names) {
                gdi.props.push_back(pair(pn, sdl_get_property_as_string(props_id, pn.c_str())));
            }
        }
        ri.gpu_device_info = MOVE(gdi);
    }
    return ri;
}
