#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <JobSystem.h>
#include <Assert.h>

#include "VulkanState.h"

// class TestJob : public Job
// {
// public:
//     explicit TestJob(size_t i, int name) : counter(i), name(name) {};
//
//     CompletionPolicy Execute() override
//     {
//         for (int i = 0; i < counter; i++)
//         {
//             {
//                 std::cout << name << " called " << i << std::endl;
//
//             }
//         }
//         return CompletionPolicy::Complete;
//     }
//
// private:
//     size_t counter;
//     int name;
// };

int main(void)
{
    // Init SDL
    DEBUG_ASSERT(SDL_Init(SDL_INIT_VIDEO));
    atexit(SDL_Quit);
    DEBUG_ASSERT(SDL_Vulkan_LoadLibrary(nullptr));
    atexit(SDL_Vulkan_UnloadLibrary);

    // Init Vulkan state
    VulkanState vulkanState;

    // Init SDL window
    SDL_Window *window = SDL_CreateWindow("VulkanRayTracer", 1600, 900,
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
    DEBUG_ASSERT(window);

    bool running = true;
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
    }

    // std::cout << "Returns " << result << std::endl;
    //
    // int i = 1, j = 2, k = 3;
    // JobSystem::GetInstance().Schedule<TestJob>(i, i);
    // JobSystem::GetInstance().Schedule<TestJob>(j, j);
    // JobSystem::GetInstance().ScheduleImportant<TestJob>(k, k);
    //
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SDL_DestroyWindow(window);

    return 0;
}
