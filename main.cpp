#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <include/JobSystem.h>
#include <Assert.h>
#include <include/JobSystem.h>
#include <include/Window.h>

#include <include/ShaderCompiler.h>



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

    // Initi glslang shader compiler
    shader_compiler::Initialize();

    // Init SDL window
    Window window;

    // Init Vulkan state
    window.Run();

    // std::cout << "Returns " << result << std::endl;
    //
    // int i = 1, j = 2, k = 3;
    // JobSystem::GetInstance().Schedule<TestJob>(i, i);
    // JobSystem::GetInstance().Schedule<TestJob>(j, j);
    // JobSystem::GetInstance().ScheduleImportant<TestJob>(k, k);
    //
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    shader_compiler::Finalize();

    return 0;
}
