#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <Debug.h>
#include <Singleton.h>
#include <include/VulkanBuffer.h>

#include "Light.h"

inline constexpr size_t MAX_LIGHT_COUNT = 16;

struct alignas(16) LightsData {
    int32_t lightCount = 0;
    int32_t padding0   = 0;
    int32_t padding1   = 0;
    int32_t padding2   = 0;
    Light   lights[MAX_LIGHT_COUNT];
};

class LightManager : public Singleton<LightManager> {
public:
    void Init(VkPhysicalDevice physicalDevice, VkDevice device);

    void Destroy();

    void Update();

    void UpdateLight(uint32_t index, Light &light);

    void AddLight(LightType type);

    void RemoveLight(size_t index);

    [[nodiscard]] const VkBuffer &GetBuffer() const { return m_buffer.GetBuffer(); }

    [[nodiscard]] size_t GetLightCount() const { return m_lights.size(); }

    [[nodiscard]] const Light &GetLight(size_t index) const {
        DEBUG_ASSERT(index < m_lights.size());
        return m_lights[index];
    }

protected:
    LightManager() = default;

    ~LightManager() = default;

private:
    std::vector<Light> m_lights;
    bool               m_directionalLight = false;
    VulkanBuffer       m_buffer;
};
