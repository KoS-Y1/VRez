#pragma once

#include <vulkan\vulkan.h>

#include <SDL3/SDL_video.h>

class UI
{
public:
    UI() = delete;

    UI(SDL_Window *window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue,
       VkDescriptorPool descriptorPool);

    ~UI() = default;

};
