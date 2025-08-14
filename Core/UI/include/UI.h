#pragma once

#include <vulkan/vulkan.h>

enum class LightType : uint32_t;
class MeshInstance;

class UI {
public:
    UI() = delete;

    UI(VkQueue queue, VkDescriptorPool descriptorPool);

    ~UI() = default;

    void TransformationWindow(MeshInstance &instance, bool &uniformScale, size_t id);
    void CameraWindow();
    void LightsWindow();

private:
    int32_t LightSection(size_t idx);
};
