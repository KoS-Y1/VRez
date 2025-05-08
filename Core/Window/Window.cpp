#include "Window.h"

#include <Assert.h>

#include <VulkanState.h>

Window::Window()
{
    CreateWindow();
}

Window::~Window()
{
    SDL_DestroyWindow(window);
}

void Window::CreateWindow()
{
    window = SDL_CreateWindow("VulkanRayTracer", WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;
    DEBUG_ASSERT(window);
}

void Window::Run()
{
    SDL_Log("SDL Window(%dx%d) running", WINDOW_WIDTH, WINDOW_HEIGHT);
    running = true;

    VulkanState vulkanState(window, width, height);

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }

        vulkanState.Present();

    }
    SDL_Log("SDL Window(%dx%d) quitting", WINDOW_WIDTH, WINDOW_HEIGHT);
}
