#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glslang/Public/ShaderLang.h>

#include <Debug.h>
#include <include/Window.h>
#include <include/VulkanState.h>
#include <include/UIRenderer.h>


int main(void)
{
    // Init SDL
    DEBUG_ASSERT(SDL_Init(SDL_INIT_VIDEO));
    atexit(SDL_Quit);
    DEBUG_ASSERT(SDL_Vulkan_LoadLibrary(nullptr));
    atexit(SDL_Vulkan_UnloadLibrary);

    // Initi glslang shader compiler
    glslang::InitializeProcess();

    Window::GetInstance().Run();


    glslang::FinalizeProcess();

    return 0;
}
