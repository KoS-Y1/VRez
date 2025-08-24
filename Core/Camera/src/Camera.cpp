#include "include/Camera.h"

#include <algorithm>

 Camera::Camera() {
     Reset();

     PracticalCascadeSplits();

     VulkanBuffer buffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
     m_buffer = std::move(buffer);
}

void Camera::Destroy() {
    m_buffer = {};
}

void Camera::Update() {
    GetFrustumCorners();

    CameraData data = {};
    data.view       = GetViewMatrix();
    data.projection = GetProjectonMatrix();
    data.position   = m_location;
    data.splits     = glm::vec3(m_splits[1], m_splits[2], m_splits[3]);

    m_buffer.Upload(sizeof(CameraData), &data);
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

void Camera::PracticalCascadeSplits() {
    m_splits[0]     = NEAR;
    float nearPlane = NEAR;
    float farPlane  = FAR;

    for (size_t i = 1; i < CASCADES_NUM; ++i) {
        float u       = static_cast<float>(i) / static_cast<float>(CASCADES_NUM);
        float log     = nearPlane * std::pow(farPlane / nearPlane, u); // Logarithmic
        float uniform = nearPlane + (farPlane - nearPlane) * u;        // Uniform
        m_splits[i]   = glm::mix(uniform, log, CASCADE_LAMBDA);        // Practical blend
    }

    m_splits[CASCADES_NUM] = FAR;
}

void Camera::GetFrustumCorners() {
    m_frustumCorners.clear();
    for (size_t i = 0; i < CASCADES_NUM; ++i) {
        m_frustumCorners.push_back(CalculateFrustumCornersWorldSpace(m_splits[i], m_splits[i + 1]));
    }
}

std::array<glm::vec3, FRUSTUM_CORNER_NUM> Camera::CalculateFrustumCornersWorldSpace(float nearPlane, float farPlane) {
    const glm::vec2 ndcXY[4] = {
        {-1.0f, 1.0f }, // top-left
        {1.0f,  1.0f }, // top-right
        {1.0f,  -1.0f}, // bottom-right
        {-1.0f, -1.0f}  // bottom-left
    };

    std::array<glm::vec3, FRUSTUM_CORNER_NUM> corners;

    const glm::mat4 inverse = glm::inverse(GetProjectonMatrix() * GetViewMatrix());

    const size_t cornerPerPlane = 4;

    // Near plane
    for (size_t i = 0; i < cornerPerPlane; ++i) {
        glm::vec4 pClip(ndcXY[i].x, ndcXY[i].y, nearPlane, 1.0f);
        glm::vec4 pWorld = inverse * pClip;
        corners[i]       = glm::vec3(pWorld) / pWorld.w;
    }

    // Far plane
    for (size_t i = 0; i < cornerPerPlane; ++i) {
        glm::vec4 pClip(ndcXY[i].x, ndcXY[i].y, farPlane, 1.0f);
        glm::vec4 pWorld = inverse * pClip;
        corners[4 + i]   = glm::vec3(pWorld) / pWorld.w;
    }

    return corners;
}
