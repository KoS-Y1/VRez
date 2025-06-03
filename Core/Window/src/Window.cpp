#include "include/Window.h"

#include "imgui.h"

#include <Debug.h>

#include <include/VulkanState.h>
#include <include/UI.h>

#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

Window::Window()
{
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

    UI myUI(m_window,
            vulkanState.GetInstance(),
            vulkanState.GetPhysicalDevice(),
            vulkanState.GetDevice(),
            vulkanState.GetQueue(),
            vulkanState.GetImGuiDescriptorPool());

    while (running)
    {
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
            ImGui_ImplSDL3_ProcessEvent(&event);
        }
        // imgui new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //some imgui UI to test
        ImGui::ShowDemoWindow();

        //make imgui calculate internal draw structures
        ImGui::Render();

        vulkanState.Present();
    }
    SDL_Log("SDL Window(%dx%d) quitting", WINDOW_WIDTH, WINDOW_HEIGHT);

    vulkanState.WaitIdle();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

}
