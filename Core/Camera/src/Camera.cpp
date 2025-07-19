#include "include/Camera.h"

#include <algorithm>

void Camera::SetFov(float fov)
{
    m_fov = fov;

    m_fov = glm::clamp(m_fov, MIN_FOV, MAX_FOV);
}

void Camera::SetLocation(glm::vec3 location)
{
    m_location = location;
}

void Camera::SetRotation(glm::vec3 pitchYawRoll)
{
    m_pitchYawRoll = pitchYawRoll;
    UpdateCameraVectors();
}


void Camera::ProcessMovement(CameraMoveDirection direction, float deltaTime)
{
    switch (direction)
    {
        case CameraMoveDirection::FORWARD:
            m_location += m_front * deltaTime;
            break;
        case CameraMoveDirection::BACKWARD:
            m_location -= m_front * deltaTime;
            break;
        case CameraMoveDirection::LEFT:
            m_location -= m_right * deltaTime;
            break;
        case CameraMoveDirection::RIGHT:
            m_location += m_right * deltaTime;
            break;
        case CameraMoveDirection::UP:
            m_location += m_up * deltaTime;
            break;
        case CameraMoveDirection::DOWN:
            m_location -= m_up * deltaTime;
            break;
        default:
            break;
    }
}

void Camera::ProcessRotation(float xOffset, float yOffset)
{
    xOffset *= m_sensity;
    yOffset *= m_sensity;

    float yaw = m_pitchYawRoll.y;
    float pitch = m_pitchYawRoll.x;

    yaw += xOffset;
    pitch -= yOffset;

    // Clamp
    pitch = std::clamp(pitch, -glm::radians(PITCH_BOUND), glm::radians(PITCH_BOUND));

    m_pitchYawRoll.x = pitch;
    m_pitchYawRoll.y = yaw;

    UpdateCameraVectors();
}

void Camera::ProcessZoom(float offset)
{
    float fov = m_fov - offset;
    SetFov(fov);
}

void Camera::Reset()
{
    m_location = glm::vec3(0.0f, 0.0f, 0.0f);
    m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_pitchYawRoll = glm::vec3(glm::radians(PITCH), glm::radians(YAW), glm::radians(ROLL));

    UpdateCameraVectors();

    m_fov = MAX_FOV;

    m_speed = 0.005f;
    m_sensity = 0.1f;
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    float pitch = m_pitchYawRoll.x;
    float yaw = m_pitchYawRoll.y;

    front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
