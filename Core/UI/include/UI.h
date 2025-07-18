#pragma once

#include <map>

#include <vulkan/vulkan.h>
#include <SDL3/SDL_video.h>

class MeshInstance;

class UI
{
public:
    UI() = delete;

    UI(SDL_Window *window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue,
       VkDescriptorPool descriptorPool);

    ~UI() = default;

    void TransformationMenu(MeshInstance &instance, bool &uniformScale);
    void CameraMenu();

};
