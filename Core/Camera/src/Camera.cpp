#include "include/Camera.h"

void Camera::CalculateViewMatrix()
{
    m_view = glm::lookAt(m_positon, m_positon + m_front, m_up);
}
