#pragma once


#include <SDL3/SDL_video.h>
#include <vulkan/vulkan.h>

#include <glm/vec3.hpp>

enum class LightType : uint32_t;
class MeshInstance;

class UI {
public:
    UI() = delete;

    UI(SDL_Window *window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkDescriptorPool descriptorPool);

    ~UI() = default;

    void TransformationWindow(MeshInstance &instance, bool &uniformScale, size_t id);
    void CameraWindow();
    void LightsWindow();

private:
    int32_t LightSection(size_t idx);
};
