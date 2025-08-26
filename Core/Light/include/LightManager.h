#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <Debug.h>
#include <Singleton.h>

#include "Light.h"
#include "include/Camera.h"

inline constexpr size_t MAX_LIGHT_COUNT = 16;

struct alignas(16) LightsData {
    int32_t                  lightCount = 0;
    [[maybe_unused]] int32_t padding0   = 0;
    [[maybe_unused]] int32_t padding1   = 0;
    [[maybe_unused]] int32_t padding2   = 0;
    glm::mat4                lightSpaceMatrix[CASCADES_NUM];
    Light                    lights[MAX_LIGHT_COUNT];
};

class LightManager : public Singleton<LightManager> {
public:
    LightsData Update();

    void UpdateLight(uint32_t index, Light &light);

    void AddLight(LightType type);

    void RemoveLight(size_t index);

    [[nodiscard]] size_t GetLightCount() const { return m_lights.size(); }

    [[nodiscard]] const Light &GetLight(size_t index) const {
        DEBUG_ASSERT(index < m_lights.size());
        return m_lights[index];
    }

protected:
    LightManager();

    ~LightManager();

private:
    std::vector<Light> m_lights;
};
