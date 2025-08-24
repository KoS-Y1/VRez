#include "include/VulkanState.h"


#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan.h>

#include <Debug.h>
#include <include/Window.h>
#include <include/ThreadPool.h>
#include <include/TextureManager.h>
#include <include/PipelineManager.h>
#include <include/MeshManager.h>
#include <include/MaterialRegistry.h>
#include <include/ObjectRegistry.h>

int main(void)
{
    // Init SDL
    DEBUG_ASSERT(SDL_Init(SDL_INIT_VIDEO));
    atexit(SDL_Quit);
    DEBUG_ASSERT(SDL_Vulkan_LoadLibrary(nullptr));
    atexit(SDL_Vulkan_UnloadLibrary);

    // Initi glslang shader compiler
    glslang::InitializeProcess();

    VulkanState::GetInstance().Init();

    PipelineManager::GetInstance().Init();
    MeshManager::GetInstance().Init();
    TextureManager::GetInstance().Init();
    ThreadPool::GetInstance().WaitIdle();

    MaterialRegistry::GetInstance().Init();
    ObjectRegistry::GetInstance().Init();

    Window::GetInstance().Run();

    ObjectRegistry::GetInstance().Destroy();
    MaterialRegistry::GetInstance().Destroy();
    TextureManager::GetInstance().Destroy();
    MeshManager::GetInstance().Destroy();
    PipelineManager::GetInstance().Destroy();

    glslang::FinalizeProcess();

    return 0;
}
