#include "include/VulkanPrefab.h"

#include <glm/gtx/euler_angles.hpp>

#include <include/ObjectRegistry.h>
#include <include/VulkanState.h>

VulkanPrefab::VulkanPrefab(const std::string& key, glm::vec3 location) : m_originalLocation(location) {
    m_object = ObjectRegistry::GetInstance().Load(key);

    Reset();
}

void VulkanPrefab::Swap(VulkanPrefab &other) noexcept {
    std::swap(m_object, other.m_object);

    std::swap(m_transformation, other.m_transformation);
    std::swap(m_location, other.m_location);
    std::swap(m_scale, other.m_scale);
    std::swap(m_rotation, other.m_rotation);
    std::swap(m_pitchYawRoll, other.m_pitchYawRoll);
    std::swap(m_originalLocation, other.m_originalLocation);
}

void VulkanPrefab::SetLocation(glm::vec3 location) {
    m_location = location;
    UpdateTransformation();
}

void VulkanPrefab::SetScale(glm::vec3 scale) {
    m_scale = scale;
    UpdateTransformation();
}

void VulkanPrefab::SetRotation(glm::quat rotation) {
    m_rotation     = rotation;
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    UpdateTransformation();
}

void VulkanPrefab::SetRotation(glm::vec3 pitchYawRoll) {
    m_pitchYawRoll = pitchYawRoll;
    m_rotation     = glm::quat_cast(glm::yawPitchRoll(m_pitchYawRoll.y, m_pitchYawRoll.x, m_pitchYawRoll.z));
    UpdateTransformation();
}

void VulkanPrefab::Reset() {
    m_transformation = glm::mat4(1.0f);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m_transformation, m_scale, m_rotation, m_location, skew, perspective);
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    m_location     = m_originalLocation;

    UpdateTransformation();
}

void VulkanPrefab::UpdateTransformation() {
    m_transformation = glm::translate(glm::mat4(1.0f), m_location) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
}



void VulkanPrefab::BindAndDraw(VkPipelineLayout pipeline) const {
    vkCmdPushConstants(VulkanState::GetInstance().GetCommandBuffer(), pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(m_transformation), &m_transformation);

    m_object->BindAndDraw(pipeline);
}

void VulkanPrefab::BindAndDrawMesh(VkPipelineLayout pipeline) const {
    vkCmdPushConstants(VulkanState::GetInstance().GetCommandBuffer(), pipeline, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(m_transformation), &m_transformation);

    m_object->BindAndDrawMesh(pipeline);}