#include "include/Window.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include <Debug.h>

#include <include/VulkanState.h>
#include <include/Camera.h>

Window::Window()
{
    m_lastTime = SDL_GetTicks();
    CreateWindow();
}

Window::~Window()
{
    SDL_DestroyWindow(m_window);
}

void Window::CreateWindow()
{
    m_window = SDL_CreateWindow("VulkanRayTracer", WINDOW_WIDTH, WINDOW_HEIGHT,
                                SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
    m_width = WINDOW_WIDTH;
    m_height = WINDOW_HEIGHT;
    DEBUG_ASSERT(m_window);
}

void Window::Run()
{
    SDL_Log("SDL Window(%dx%d) running", WINDOW_WIDTH, WINDOW_HEIGHT);
    running = true;

    VulkanState vulkanState(m_window, m_width, m_height);

    while (running)
    {
        uint32_t time = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
            // Process input
            Camera::GetInstance().ProcessMovement(ProcessCameraMovement(event), (time - m_lastTime) / 1000.0f);

            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        // imgui new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();


        vulkanState.ShowUI();

        //make imgui calculate internal draw structures
        ImGui::Render();
        auto view = Camera::GetInstance().GetViewMatrix();
        vulkanState.UpdatView( Camera::GetInstance().GetViewMatrix(), glm::mat4(1.0f));
        vulkanState.Present();

        m_lastTime = time;
    }
    SDL_Log("SDL Window(%dx%d) quitting", WINDOW_WIDTH, WINDOW_HEIGHT);

    vulkanState.WaitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

CameraMoveDirection Window::ProcessCameraMovement(const SDL_Event &event)
{
    if(event.type != SDL_EVENT_KEY_DOWN)
    {
        return CameraMoveDirection::STAY;
    }
    switch (event.key.scancode)
    {
        case SDL_SCANCODE_W:
            SDL_Log("Forward");
            return CameraMoveDirection::FORWARD;
        case SDL_SCANCODE_S:
            SDL_Log("Backward");
            return CameraMoveDirection::BACKWARD;
        case SDL_SCANCODE_A:
            SDL_Log("Left");
            return CameraMoveDirection::LEFT;
        case SDL_SCANCODE_D:
            SDL_Log("Right");
            return CameraMoveDirection::RIGHT;
        case SDL_SCANCODE_E:
            SDL_Log("Up");
            return CameraMoveDirection::UP;
        case SDL_SCANCODE_Q:
            SDL_Log("Down");
            return CameraMoveDirection::DOWN;
        default:
            return CameraMoveDirection::STAY;
    }

}
