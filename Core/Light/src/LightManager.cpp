#include "include/LightManager.h"


LightManager::~LightManager() {
    m_lights.clear();
}

LightsData LightManager::Update() {
    LightsData data = {};

    data.lightCount = m_lights.size();
    for (size_t i = 0; i < m_lights.size(); i++) {
        data.lights[i] = m_lights[i];
    }

    return data;
}

void LightManager::UpdateLight(uint32_t index, Light &light) {
    m_lights[index] = std::move(light);
}

void LightManager::AddLight(LightType type) {
    if (m_lights.size() >= MAX_LIGHT_COUNT) {
        return;
    }

    // Only one directional light
    if (m_directionalLight && type == LightType::Directional) {
        return;
    }

    if (type == LightType::Directional) {
        m_directionalLight = true;
    }

    Light light;
    light.type = static_cast<uint32_t>(type);
    m_lights.push_back(light);
}

void LightManager::RemoveLight(size_t index) {
    if (m_lights[index].type == static_cast<uint32_t>(LightType::Directional)) {
        m_directionalLight = false;
    }
    m_lights.erase(m_lights.begin() + index);
}