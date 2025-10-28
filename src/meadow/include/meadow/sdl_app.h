#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

class SDLApp
{
public:
    virtual ~SDLApp() = default;

    virtual SDL_AppResult SDL_AppIterate() = 0;
    virtual SDL_AppResult SDL_AppEvent(SDL_Event* event) = 0;
    virtual void SDL_AppQuit(SDL_AppResult result) = 0;
};

// make_sdl_app must be implemented in the consuming project.
SDLApp* make_sdl_app(int argc, char** argv);
