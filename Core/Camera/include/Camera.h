#pragma once

#include <glm/glm.hpp>

#include "glm/ext/matrix_transform.hpp"

class Camera
{
public:
    Camera()
    {
        m_positon = glm::vec3(0.0f, 0.0f, 0.0f);
        m_front = glm::vec3(0.0f, 0.0f, 1.0f);
        m_up = glm::vec3(0.0f, -1.0f, 0.0f);
        CalculateViewMatrix();

        m_speed = 2.5f;
    }

    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float speed)
    {
        m_positon = position;
        m_front = front;
        m_up = up;
        CalculateViewMatrix();

        m_speed = speed;
    }

    ~Camera() = default;

    Camera(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera& operator=(Camera&&) = delete;

    [[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_view; }


private:
    glm::vec3 m_positon;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::mat4 m_view;

    float m_speed;

    void CalculateViewMatrix();
};
