#include "include/Window.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include <Debug.h>

#include <include/Camera.h>
#include <include/LightManager.h>
#include <include/PbrRenderer.h>
#include <include/PipelineManager.h>
#include <include/UIRenderer.h>
#include <include/VulkanState.h>

Window::Window() {
    m_lastTime = SDL_GetTicks();
    CreateWindow();
}

Window::~Window() {
    SDL_DestroyWindow(m_window);
}

void Window::CreateWindow() {
    m_window = SDL_CreateWindow("VulkanRayTracer", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);

    m_width  = WINDOW_WIDTH;
    m_height = WINDOW_HEIGHT;
    DEBUG_ASSERT(m_window);
}

void Window::Run() {
    SDL_Log("SDL Window(%dx%d) running", WINDOW_WIDTH, WINDOW_HEIGHT);
    m_running = true;

    UIRenderer  uiRenderer;
    PbrRenderer pbrRenderer;

    while (m_running) {
        uint32_t  time = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                m_running = false;
                break;
            default:
                break;
            }
            // Process input
            ProcessCamera(event, static_cast<float>(time - m_lastTime) / 1000.0f);
            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        // imgui new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        uiRenderer.Present();
        ImGui::Render();

        VulkanState::GetInstance().BeginFrame();

        pbrRenderer.Render();
        uiRenderer.Render();

        VulkanState::GetInstance().EndFrame();

        m_lastTime = time;
    }
    SDL_Log("SDL Window(%dx%d) quitting", WINDOW_WIDTH, WINDOW_HEIGHT);

    VulkanState::GetInstance().WaitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void Window::ProcessCamera(const SDL_Event &event, float deltaTime) {
    ProcessCameraKeyboard(event, deltaTime);
    ProcessCameraMouse();
    ProcessCameraScroll(event);
}

void Window::ProcessCameraKeyboard(const SDL_Event &event, float deltaTime) {
    if (event.type != SDL_EVENT_KEY_DOWN) {
        return;
    }

    switch (event.key.scancode) {
    case SDL_SCANCODE_W:
        Camera::GetInstance().ProcessMovement(CameraMoveDirection::FORWARD, deltaTime);
        break;
    case SDL_SCANCODE_S:
        Camera::GetInstance().ProcessMovement(CameraMoveDirection::BACKWARD, deltaTime);
        break;
    case SDL_SCANCODE_A:
        Camera::GetInstance().ProcessMovement(CameraMoveDirection::LEFT, deltaTime);
        break;
    case SDL_SCANCODE_D:
        Camera::GetInstance().ProcessMovement(CameraMoveDirection::RIGHT, deltaTime);
        break;
    case SDL_SCANCODE_E:
        Camera::GetInstance().ProcessMovement(CameraMoveDirection::UP, deltaTime);
        break;
    case SDL_SCANCODE_Q:
        Camera::GetInstance().ProcessMovement(CameraMoveDirection::DOWN, deltaTime);
        break;
    // Camera mode
    case SDL_SCANCODE_LSHIFT:
        if (m_cameraMode) {
            m_cameraMode = false;
        } else {
            m_cameraMode = true;
            m_firstMouse = true;
        }
        SDL_Log("Camera mood = %s", m_cameraMode ? "ON" : "OFF");
    default:
        break;
    }
}

void Window::ProcessCameraMouse() {
    if (!m_cameraMode) {
        return;
    }
    float xPos, yPos;
    SDL_GetMouseState(&xPos, &yPos);

    if (m_firstMouse) {
        m_mouseX     = xPos;
        m_mouseY     = yPos;
        m_firstMouse = false;
    }

    Camera::GetInstance().ProcessRotation(xPos - m_mouseX, yPos - m_mouseY);

    m_mouseX = xPos;
    m_mouseY = yPos;
}

void Window::ProcessCameraScroll(const SDL_Event &event) {
    if (!m_cameraMode || event.type != SDL_EVENT_MOUSE_WHEEL) {
        return;
    }

    Camera::GetInstance().ProcessZoom(event.wheel.y);
}
