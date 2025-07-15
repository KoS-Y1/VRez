#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/detail/type_quat.hpp>

#include <Singleton.h>

#define YAW                 (-90.0f)
#define PITCH               0.0f
#define ROLL                0.0f
#define PITCH_BOUND         89.0f

#define MAX_FOV            45.0f
#define MIN_FOV            1.0f

#define RATIO               (16.0f / 9.0f)
#define NEAR                0.1f
#define FAR                 100.0f

enum class CameraMoveDirection : uint8_t
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

// Camera class as a singleton, since we only have 1 camera
class Camera : public Singleton<Camera>
{
public:
    void ProcessMovement(CameraMoveDirection direction, float deltaTime);

    void ProcessRotation(float xOffset, float yOffset);

    void ProcessZoom(float offset);

    void SetFov(float fov);

    void SetLocation(glm::vec3 location);

    void SetRotation(glm::vec3 pitchYawRoll);

    void Reset();

    [[nodiscard]] glm::mat4 GetViewMatrix() const { return glm::lookAt(m_location, m_location + m_front, m_up); }

    [[nodiscard]] glm::mat4 GetProjectonMatrix() const
    {
        return glm::perspective(glm::radians(m_fov), RATIO, NEAR, FAR);
    }

    [[nodiscard]] float GetFOV() const { return m_fov; }
    [[nodiscard]] glm::vec3 GetPitchYawRoll() const { return m_pitchYawRoll; }
    [[nodiscard]] glm::vec3 GetLocation() const { return m_location; }

protected:
    Camera() { Reset(); }

    ~Camera() = default;

private:
    glm::vec3 m_location;
    glm::vec3 m_worldUp;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_pitchYawRoll; // In radians

    float m_fov;

    float m_sensity;
    float m_speed;

    void UpdateCameraVectors();
};
