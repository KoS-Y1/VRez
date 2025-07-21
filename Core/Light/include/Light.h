#pragma once

#include <glm/glm.hpp>

enum class LightType : uint32_t
{
    Point = 0,
    Directional,
    Ambient,
    Spot
};

struct alignas(16) Light
{
public:
    glm::vec3 color = glm::vec3(1.0f);
    uint32_t type = static_cast<uint32_t>(LightType::Point);

    glm::vec3 position = glm::vec3(0.0f);
    float range = 10.0f; // Point/spot falloff

    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f); // Directional/spot direction
    float innerAngle = 0.0f; // Spot inner angle, in degrees

    float outerAngle = 0.0f; // Spot outter angle, in degrees
    float intensity = 1.0f;
    int32_t padding0 = 0;
    int32_t padding1 = 0;
};
