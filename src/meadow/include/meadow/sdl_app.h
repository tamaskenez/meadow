#pragma once

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

#include <memory>

class SDLApp
{
public:
    virtual ~SDLApp() = default;

    virtual SDL_AppResult SDL_AppIterate() = 0;
    virtual SDL_AppResult SDL_AppEvent(SDL_Event* event) = 0;
    virtual void SDL_AppQuit(SDL_AppResult result) = 0;
};

// make_sdl_app must be implemented in the consuming project.
extern SDLApp* make_sdl_app(int argc, char** argv);

inline SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    if (auto* sdl_app = make_sdl_app(argc, argv)) {
        *appstate = static_cast<void*>(sdl_app);
        return SDL_APP_CONTINUE;
    } else {
        return SDL_APP_FAILURE;
    }
}

inline SDL_AppResult SDL_AppIterate(void* appstate)
{
    return reinterpret_cast<SDLApp*>(appstate)->SDL_AppIterate();
}

inline SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    return reinterpret_cast<SDLApp*>(appstate)->SDL_AppEvent(event);
}

inline void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto app = std::unique_ptr<SDLApp>(reinterpret_cast<SDLApp*>(appstate));
    app->SDL_AppQuit(result);
}
