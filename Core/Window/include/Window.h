#pragma once
#include "SDL3/SDL_video.h"

#include <Singleton.h>

#include "SDL3/SDL_events.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

enum class CameraMoveDirection :uint8_t;

class Window : public Singleton<Window>
{
public:

    void Run();

protected:
    Window();

    ~Window();

private:
    SDL_Window *m_window = nullptr;

    uint32_t m_width = 0;
    uint32_t m_height = 0;

    uint32_t m_lastTime;

    bool running = false;

    void CreateWindow();

    CameraMoveDirection ProcessCameraMovement(const SDL_Event &event);
};
