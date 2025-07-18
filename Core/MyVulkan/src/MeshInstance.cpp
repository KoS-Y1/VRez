#include "include/MeshInstance.h"

#include <include/VulkanMesh.h>

#include "glm/gtx/euler_angles.hpp"
#include "include/VulkanGraphicsPipeline.h"

MeshInstance::MeshInstance(const VulkanMesh *mesh, std::shared_ptr<VulkanGraphicsPipeline> pipeline)
{
    m_mesh = mesh;
    m_pipeline = pipeline;
    m_transformation = glm::mat4(1.0f);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m_transformation, m_scale, m_rotation, m_location, skew, perspective);
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
}

MeshInstance::MeshInstance(const VulkanMesh *mesh, std::shared_ptr<VulkanGraphicsPipeline> pipeline, glm::vec3 location,
                           glm::quat rotation, glm::vec3 scale)
{
    m_mesh = mesh;
    m_pipeline = pipeline;
    m_location = location;
    m_rotation = rotation;
    m_scale = scale;
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    UpdateTransformation();
}

MeshInstance::MeshInstance(const VulkanMesh *mesh, std::shared_ptr<VulkanGraphicsPipeline> pipeline, glm::vec3 location,
                           glm::vec3 pitchYawRoll, glm::vec3 scale)
{
    m_mesh = mesh;
    m_pipeline = pipeline;
    m_location = location;
    m_pitchYawRoll = pitchYawRoll;
    m_rotation = glm::quat_cast(glm::yawPitchRoll(m_pitchYawRoll.y, m_pitchYawRoll.x, m_pitchYawRoll.z));
    UpdateTransformation();
}

void MeshInstance::Destroy()
{
    m_mesh = nullptr;
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
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    UpdateTransformation();
}

void MeshInstance::SetRotation(glm::vec3 pitchYawRoll)
{
    m_pitchYawRoll = pitchYawRoll;
    m_rotation = glm::quat_cast(glm::yawPitchRoll(m_pitchYawRoll.y, m_pitchYawRoll.x, m_pitchYawRoll.z));
    UpdateTransformation();
}


void MeshInstance::Reset()
{
    m_transformation = glm::mat4(1.0f);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m_transformation, m_scale, m_rotation, m_location, skew, perspective);
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
}

void MeshInstance::UpdateTransformation()
{
    m_transformation = glm::translate(glm::mat4(1.0f), m_location) * glm::mat4_cast(m_rotation) * glm::scale(
                           glm::mat4(1.0f), m_scale);
    m_transformation = glm::scale(m_transformation, glm::vec3(0.1f, 0.1f, 0.1f));
}

void MeshInstance::BindAndDraw(VkCommandBuffer cmdBuf) const
{
    const VkDeviceSize offset = 0;

    vkCmdBindVertexBuffers(cmdBuf, 0, 1, &m_mesh->GetVertexBuffer(), &offset);
    vkCmdPushConstants(cmdBuf, m_pipeline->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(m_transformation), &m_transformation);
    vkCmdDraw(cmdBuf, m_mesh->GetVertexCount(), 1, 0, 0);
}
