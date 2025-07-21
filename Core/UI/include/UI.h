#pragma once


#include <vulkan/vulkan.h>
#include <SDL3/SDL_video.h>

#include <glm/vec3.hpp>

enum class LightType : uint32_t;
class MeshInstance;


class UI
{
public:
    UI() = delete;

    UI(SDL_Window *window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue,
       VkDescriptorPool descriptorPool);

    ~UI() = default;

    void TransformationWindow(MeshInstance &instance, bool &uniformScale);
    void CameraWindow();
    void LightsWindow();

private:
    void LightSection(size_t idx);
};
