#include "include/Camera.h"

#include "glm/gtc/quaternion.hpp"

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

void Camera::ProcessMovement(CameraMoveDirection direction, float deltaTime)
{
    switch (direction)
    {
        case CameraMoveDirection::FORWARD:
            m_positon += m_front * deltaTime;
            break;
        case CameraMoveDirection::BACKWARD:
            m_positon -= m_front * deltaTime;
            break;
        case CameraMoveDirection::LEFT:
            m_positon -= m_right * deltaTime;
            break;
        case CameraMoveDirection::RIGHT:
            m_positon += m_right * deltaTime;
            break;
        case CameraMoveDirection::UP:
            m_positon += m_up * deltaTime;
            break;
        case CameraMoveDirection::DOWN:
            m_positon -= m_up * deltaTime;
            break;
        case CameraMoveDirection::STAY:
        default:
            break;
    }
    UpdateCameraVectors();
}
