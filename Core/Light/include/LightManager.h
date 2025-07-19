#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <Singleton.h>

#include "Light.h"

#define MAX_LIGHTS 16

class VulkanBuffer;

class LightManager : public Singleton<LightManager>
{
public:
    void Bind(VkCommandBuffer cmdBuf);

    // TODO
    void UpdateLight(uint32_t index);
    void AddLight(LightType type);

protected:
    LightManager() = default;

    ~LightManager() = default;

private:
    std::vector<Light> m_lights;
    VulkanBuffer m_buffer;
};
