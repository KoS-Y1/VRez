#include "include/LightManager.h"

#include <include/LightUtil.h>

namespace {
constexpr size_t DIRECTIONAL_INDEX = 0;
} // namespace

LightManager::LightManager() {
    AddLight(LightType::Directional);
}

LightManager::~LightManager() {
    m_lights.clear();
}

LightsData LightManager::Update() {
    LightsData data = {};

    data.lightCount = m_lights.size();
    for (size_t i = 0; i < m_lights.size(); ++i) {
        data.lights[i] = m_lights[i];
    }

  data.lightSpaceMatrix = light_util::GetLightSpaceMatrix(m_lights[DIRECTIONAL_INDEX].direction);

    return data;
}

void LightManager::UpdateLight(uint32_t index, Light &light) {
    m_lights[index] = std::move(light);
}

void LightManager::AddLight(LightType type) {
    if (m_lights.size() >= MAX_LIGHT_COUNT) {
        return;
    }

    Light light;
    light.type = static_cast<uint32_t>(type);
    m_lights.push_back(light);
}

void LightManager::RemoveLight(size_t index) {
    m_lights.erase(m_lights.begin() + index);
}
