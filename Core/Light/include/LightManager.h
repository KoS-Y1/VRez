#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <Singleton.h>
#include <include/VulkanBuffer.h>

#include "Light.h"

#define MAX_LIGHTS 16u

struct alignas(16) LightsData
{
    int32_t lightCount = 0;
    int32_t padding0 = 0;
    int32_t padding1 = 0;
    int32_t padding2 = 0;
    Light lights[MAX_LIGHTS];
};


class LightManager : public Singleton<LightManager>
{
public:
    void Init(VkPhysicalDevice physicalDevice, VkDevice device);

    void Destroy();

    void Update();

    // TODO
    void UpdateLight(uint32_t index);

    void AddLight(LightType type);

    [[nodiscard]] const VkBuffer &GetBuffer() const { return m_buffer.GetBuffer(); }

protected:
    LightManager() = default;

    ~LightManager() = default;

private:
    std::vector<Light> m_lights;
    VulkanBuffer m_buffer;
};
