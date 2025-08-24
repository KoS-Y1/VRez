#pragma once

#include <array>

#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <Singleton.h>
#include <include/VulkanBuffer.h>

inline constexpr size_t FRUSTUM_CORNER_NUM = 8;
inline constexpr size_t CASCADES_NUM       = 3;

enum class CameraMoveDirection : uint8_t {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

struct alignas(16) CameraData {
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;
    float     padding0;
    glm::vec3 splits;
    float     padding1;
};

// Camera class as a singleton, since we only have 1 camera
class Camera : public Singleton<Camera> {
public:
    void Destroy();

    void Update();

    void ProcessMovement(CameraMoveDirection direction, float deltaTime);

    void ProcessRotation(float xOffset, float yOffset);

    void ProcessZoom(float offset);

    void SetFov(float fov);

    void SetLocation(glm::vec3 location);

    void SetRotation(glm::vec3 pitchYawRoll);

    void Reset();

    [[nodiscard]] glm::mat4 GetViewMatrix() const { return glm::lookAt(m_location, m_location + m_front, m_up); }

    [[nodiscard]] glm::mat4 GetProjectonMatrix() const { return glm::perspective(glm::radians(m_fov), RATIO, NEAR, FAR); }

    [[nodiscard]] float GetFOV() const { return m_fov; }

    [[nodiscard]] glm::vec3 GetPitchYawRoll() const { return m_pitchYawRoll; }

    [[nodiscard]] glm::vec3 GetLocation() const { return m_location; }

    [[nodiscard]] const VkBuffer &GetBuffer() const { return m_buffer.GetBuffer(); }

protected:
    Camera();

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


    static constexpr float YAW         = -90.0f;
    static constexpr float PITCH       = 0.0f;
    static constexpr float ROLL        = 0.0f;
    static constexpr float PITCH_BOUND = 89.0f;
    static constexpr float MAX_FOV     = 45.0f;
    static constexpr float MIN_FOV     = 1.0f;
    static constexpr float DEFAULT_FOV = 15.0f;
    static constexpr float RATIO       = (16.0f / 9.0f);
    static constexpr float NEAR        = 0.001f;
    static constexpr float FAR         = 64.0f;

    static constexpr float CASCADE_LAMBDA = 0.75f;

    std::array<float, CASCADES_NUM + 1>                    m_splits;
    std::vector<std::array<glm::vec3, FRUSTUM_CORNER_NUM>> m_frustumCorners;

    VulkanBuffer m_buffer;

    void UpdateCameraVectors();

    void PracticalCascadeSplits();

    void GetFrustumCorners();

    std::array<glm::vec3, FRUSTUM_CORNER_NUM> CalculateFrustumCornersWorldSpace(float nearPlane, float farPlane);
};
