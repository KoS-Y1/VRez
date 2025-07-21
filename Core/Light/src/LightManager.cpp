#include "include/LightManager.h"


void LightManager::Init(VkPhysicalDevice physicalDevice, VkDevice device)
{
    VulkanBuffer buffer(physicalDevice, device, sizeof(LightsData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    m_buffer = std::move(buffer);
}

void LightManager::Destroy()
{
    m_lights.clear();
    m_buffer.Destroy();
}


void LightManager::Update()
{
    LightsData data = {};

    data.lightCount = m_lights.size();
    for (size_t i = 0; i < m_lights.size(); i++)
    {
        data.lights[i] = m_lights[i];
    }

    m_buffer.Upload(sizeof(LightsData), &data);
}

void LightManager::UpdateLight(uint32_t index)
{
}

void LightManager::AddLight(LightType type)
{
    if (m_lights.size() >= MAX_LIGHTS)
    {
        return;
    }

    Light light;
    light.type = static_cast<uint32_t>(type);
    m_lights.push_back(light);
}
