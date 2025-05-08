#pragma once
#include "SDL3/SDL_video.h"

#include <Singleton.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

class Window : public Singleton<Window>
{
public:
    Window();

    ~Window();

    [[nodiscard]] SDL_Window *const &GetSDLWindow() const { return window; };

    void Run();

private:
    SDL_Window *window = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;

    bool running = false;

    void CreateWindow();
};
