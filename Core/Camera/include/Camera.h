#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/detail/type_quat.hpp>

#include <Singleton.h>

#include "glm/gtx/matrix_decompose.hpp"
#define YAW     (-90.0f)
#define PITCH   0.0f
#define ROLL    0.0f

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

    [[nodiscard]] const glm::mat4 GetViewMatrix() const { return glm::lookAt(m_positon, m_positon + m_front, m_up); }

protected:
    Camera()
    {
        m_positon = glm::vec3(0.0f, 0.0f, 0.0f);
        m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        m_pitchYawRoll = glm::vec3(glm::radians(PITCH), glm::radians(YAW), glm::radians(ROLL));

        UpdateCameraVectors();

        m_speed = 0.005f;
        m_sensity = 0.1f;
    }

    ~Camera() = default;

private:
    glm::vec3 m_positon;
    glm::vec3 m_worldUp;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_pitchYawRoll; // In radians

    float m_sensity;
    float m_speed;

    void UpdateCameraVectors();
};
