#pragma once
#include "SDL3/SDL_video.h"

#include <Singleton.h>

#include <SDL3/SDL_events.h>

namespace {
constexpr int WINDOW_WIDTH  = 1600;
constexpr int WINDOW_HEIGHT = 900;
} // namespace

enum class CameraMoveDirection : uint8_t;

class Window : public Singleton<Window> {
public:
    void Run();

    [[nodiscard]] SDL_Window *GetSDLWindow() { return m_window; }

    [[nodiscard]] uint32_t GetWidth() { return m_width; }

    [[nodiscard]] uint32_t GetHeight() { return m_height; }

protected:
    Window();

    ~Window();

private:
    SDL_Window *m_window = nullptr;

    uint32_t m_width  = 0;
    uint32_t m_height = 0;

    uint32_t m_lastTime;

    bool m_running = false;

    bool  m_cameraMode = false;
    bool  m_firstMouse = true;
    float m_mouseX     = 0.0f;
    float m_mouseY     = 0.0f;

    void CreateWindow();

    void ProcessCamera(const SDL_Event &event, float deltaTime);
    void ProcessCameraKeyboard(const SDL_Event &event, float deltaTime);
    void ProcessCameraMouse();
    void ProcessCameraScroll(const SDL_Event &event);
};
