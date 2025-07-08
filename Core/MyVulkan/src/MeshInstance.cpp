#include "include/MeshInstance.h"

#include <include/VulkanMesh.h>

#include "glm/gtx/transform.hpp"

void MeshInstance::Destroy()
{
    delete(m_mesh);
}

void MeshInstance::Swap(MeshInstance &other) noexcept
{
    m_model = other.m_model;
    std::swap(m_mesh, other.m_mesh);
}


void MeshInstance::Translate(const glm::vec3 &translation)
{
    m_model = glm::translate(m_model, translation);
}

void MeshInstance::Scale(const glm::vec3 &scale)
{
    m_model = glm::scale(m_model, scale);
}

void MeshInstance::Reset()
{
    m_model = glm::mat4(1.0f);
}
