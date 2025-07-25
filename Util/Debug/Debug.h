#pragma once
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan_core.h>

inline void Assert(const char* expression, bool condition, const char* file, int line)
{
    if (!condition)
    {
        SDL_Log("Assertion failed: %s, %s(%d)", expression, file, line);
        SDL_Log(SDL_GetError());

        exit(EXIT_FAILURE);
    }
}

#define DEBUG_ASSERT(expression) Assert(#expression, (expression), __FILE__, __LINE__)

inline void VkAssert(const char* expression, VkResult result, const char* file, int line)
{
    if (result != VK_SUCCESS)
    {
        SDL_Log("Vulkan assertion failed: %s, %s(%d), error code: %d", expression, file, line, result);
        exit(EXIT_FAILURE);
    }
}

#define DEBUG_VK_ASSERT(expression) VkAssert(#expression, (expression), __FILE__, __LINE__)