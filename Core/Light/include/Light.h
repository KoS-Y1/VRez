#pragma once

#include <glm/glm.hpp>

enum class LightType : uint32_t
{
    Point = 0,
    Directional,
    Ambient,
    Spot
};

class alignas(16) Light
{
public:
    Light() = default;

    ~Light() = default;

    Light(LightType type) { m_type = static_cast<uint32_t>(type); }

    // Light(const Light &other) = delete;
    //
    // Light(Light &&) = delete;
    //
    // Light &operator=(const Light &other)
    // {
    //     m_color = other.m_color;
    //     m_type = other.m_type;
    //     m_position = other.m_position;
    //     m_range = other.m_range;
    //     m_direction = other.m_direction;
    //     m_innerAngle = other.m_innerAngle;
    //     m_outerAngle = other.m_outerAngle;
    //     m_intensity = other.m_intensity;
    // }
    //
    // Light &operator=(Light &&) = delete;

    [[nodiscard]] LightType GetType() const { return static_cast<LightType>(m_type); }
    [[nodiscard]] glm::vec3 GetColor() const { return m_color; }
    [[nodiscard]] glm::vec3 GetPosition() const { return m_position; }
    [[nodiscard]] float GetRange() const { return m_range; }
    [[nodiscard]] glm::vec3 GetDirection() const { return m_direction; }
    [[nodiscard]] float GetInnerAngle() const { return m_innerAngle; }
    [[nodiscard]] float GetOuterAngle() const { return m_outerAngle; }
    [[nondiscard]] float GetIntensity() const { return m_intensity; }

    void SetColor(glm::vec3 color) { m_color = color; }
    void SetPosition(glm::vec3 position) { m_position = position; }
    void SetRange(float range) { m_range = range; }
    void SetDirection(glm::vec3 direction) { m_direction = direction; }
    void SetInnerAngle(float angle) { m_innerAngle = angle; }
    void SetOuterAngle(float angle) { m_outerAngle = angle; }
    void SetIntensity(float intensity) { m_intensity = intensity; }

private:
    glm::vec3 m_color = glm::vec3(1.0f);
    uint32_t m_type = static_cast<uint32_t>(LightType::Point);

    glm::vec3 m_position = glm::vec3(0.0f);
    float m_range = 10.0f; // Point/spot falloff

    glm::vec3 m_direction = glm::vec3(0.0f, -1.0f, 0.0f); // Directional/spot direction
    float m_innerAngle = 0.0f; // Spot inner angle, in degrees

    float m_outerAngle = 0.0f; // Spot outter angle, in degrees
    float m_intensity = 1.0f;
    float m_paddings[2] = {};
};
