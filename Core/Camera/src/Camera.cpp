#include "include/Camera.h"

#include <include/VulkanState.h>

#include <algorithm>

namespace {
constexpr float YAW         = -90.0f;
constexpr float PITCH       = 0.0f;
constexpr float ROLL        = 0.0f;
constexpr float PITCH_BOUND = 89.0f;
constexpr float MAX_FOV     = 45.0f;
constexpr float MIN_FOV     = 1.0f;
constexpr float DEFAULT_FOV = 15.0f;


constexpr float CASCADE_LAMBDA = 0.75f;
} // namespace

Camera::Camera() {
    Reset();
}

CameraData Camera::Update() {
    CameraData data = {};
    data.view       = GetViewMatrix();
    data.projection = GetProjectonMatrix();
    data.position   = m_location;
    data.resolution = glm::vec2(VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight());
    return data;
}

void Camera::SetFov(float fov) {
    m_fov = fov;

    m_fov = glm::clamp(m_fov, MIN_FOV, MAX_FOV);
}

void Camera::SetLocation(glm::vec3 location) {
    m_location = location;
}

void Camera::SetRotation(glm::vec3 pitchYawRoll) {
    m_pitchYawRoll = pitchYawRoll;
    UpdateCameraVectors();
}

void Camera::ProcessMovement(CameraMoveDirection direction, float deltaTime) {
    switch (direction) {
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

void Camera::ProcessRotation(float xOffset, float yOffset) {
    xOffset *= m_sensity;
    yOffset *= m_sensity;

    float yaw   = m_pitchYawRoll.y;
    float pitch = m_pitchYawRoll.x;

    yaw   += xOffset;
    pitch -= yOffset;

    // Clamp
    pitch = std::clamp(pitch, -glm::radians(PITCH_BOUND), glm::radians(PITCH_BOUND));

    m_pitchYawRoll.x = pitch;
    m_pitchYawRoll.y = yaw;

    UpdateCameraVectors();
}

void Camera::ProcessZoom(float offset) {
    float fov = m_fov - offset;
    SetFov(fov);
}

void Camera::Reset() {
    m_location     = glm::vec3(0.0f, 0.0f, 0.0f);
    m_worldUp      = glm::vec3(0.0f, 1.0f, 0.0f);
    m_pitchYawRoll = glm::vec3(glm::radians(PITCH), glm::radians(YAW), glm::radians(ROLL));

    UpdateCameraVectors();

    m_fov = DEFAULT_FOV;

    m_speed   = 0.005f;
    m_sensity = 0.001f;
}

void Camera::UpdateCameraVectors() {
    glm::vec3 front;
    float     pitch = m_pitchYawRoll.x;
    float     yaw   = m_pitchYawRoll.y;

    front.x = glm::cos(yaw) * glm::cos(pitch);
    front.y = glm::sin(pitch);
    front.z = glm::sin(yaw) * glm::cos(pitch);

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}
