#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "meadow/sdl_app.h"

#include <memory>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    if (auto* sdl_app = make_sdl_app(argc, argv)) {
        *appstate = static_cast<void*>(sdl_app);
        return SDL_APP_CONTINUE;
    } else {
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    return reinterpret_cast<SDLApp*>(appstate)->SDL_AppIterate();
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    return reinterpret_cast<SDLApp*>(appstate)->SDL_AppEvent(event);
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto app = std::unique_ptr<SDLApp>(reinterpret_cast<SDLApp*>(appstate));
    app->SDL_AppQuit(result);
}
