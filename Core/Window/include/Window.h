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

    [[nodiscard]] SDL_Window *const &GetSDLWindow() const { return m_window; };

    void Run();

private:
    SDL_Window *m_window = nullptr;

    uint32_t m_width = 0;
    uint32_t m_height = 0;

    bool running = false;

    void CreateWindow();
};
