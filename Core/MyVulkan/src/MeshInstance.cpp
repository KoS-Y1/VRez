#include "include/MeshInstance.h"

#include <include/VulkanMesh.h>



void MeshInstance::Destroy()
{
    m_mesh->Destroy();
}

void MeshInstance::Swap(MeshInstance &other) noexcept
{
    m_transformation = other.m_transformation;
    std::swap(m_mesh, other.m_mesh);
}

void MeshInstance::SetLocation(glm::vec3 location)
{
    m_location = location;
    UpdateTransformation();
}

void MeshInstance::SetScale(glm::vec3 scale)
{
    m_scale = scale;
    UpdateTransformation();
}

void MeshInstance::SetRotation(glm::quat rotation)
{
    m_rotation = rotation;
    // m_pitchYawRoll
    UpdateTransformation();
}

void MeshInstance::SetRotation(glm::vec3 pitchYawRoll)
{
    m_pitchYawRoll = pitchYawRoll;
    // m_rotation =
        UpdateTransformation();
}


void MeshInstance::Reset()
{
    m_transformation = glm::mat4(1.0f);
}

void MeshInstance::UpdateTransformation()
{
    m_transformation = glm::translate(glm::mat4(1.0f), m_location) * glm::mat4_cast(m_rotation) * glm::scale(
                           glm::mat4(1.0f), m_scale);
}
