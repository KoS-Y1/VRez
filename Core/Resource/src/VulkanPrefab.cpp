#include "include/VulkanPrefab.h"

#include <glm/gtx/euler_angles.hpp>

#include <include/VulkanGraphicsPipeline.h>
#include <include/VulkanMesh.h>
#include <include/VulkanState.h>
#include <include/VulkanUtil.h>

VulkanPrefab::VulkanPrefab(
    const VulkanMesh                       *mesh,
    const VulkanTexture                    *baseTexture,
    const VulkanTexture                    *normalMap,
    const VulkanTexture                    *ormTexture,
    const VulkanTexture                    *emissiveTexture,
    const VulkanTexture                    *brdfTexture,
    const VulkanTexture                    *skyboxSpecular,
    const VulkanTexture                    *skyboxIrradiance,
    std::shared_ptr<VulkanGraphicsPipeline> pipeline
) {
    m_mesh            = mesh;
    m_baseTexture     = baseTexture;
    m_normalMap       = normalMap;
    m_ormTexture      = ormTexture;
    m_emissiveTexture = emissiveTexture;

    m_brdfTexture      = brdfTexture;
    m_skyboxSpecular   = skyboxSpecular;
    m_skyboxIrradiance = skyboxIrradiance;

    m_pipeline = pipeline;
    CreateDescriptorSets();

    m_originalLocation = glm::vec3(0.f);
    Reset();
}

VulkanPrefab::VulkanPrefab(
    const VulkanMesh                       *mesh,
    const VulkanTexture                    *baseTexture,
    const VulkanTexture                    *normalMap,
    const VulkanTexture                    *ormTexture,
    const VulkanTexture                    *emissiveTexture,
    const VulkanTexture                    *brdfTexture,
    const VulkanTexture                    *skyboxSpecular,
    const VulkanTexture                    *skyboxIrradiance,
    std::shared_ptr<VulkanGraphicsPipeline> pipeline,
    glm::vec3                               location
) {
    m_mesh            = mesh;
    m_baseTexture     = baseTexture;
    m_normalMap       = normalMap;
    m_ormTexture      = ormTexture;
    m_emissiveTexture = emissiveTexture;

    m_brdfTexture      = brdfTexture;
    m_skyboxSpecular   = skyboxSpecular;
    m_skyboxIrradiance = skyboxIrradiance;

    m_pipeline = pipeline;
    CreateDescriptorSets();

    m_originalLocation = location;
    Reset();
}

VulkanPrefab::VulkanPrefab(
    const VulkanMesh                       *mesh,
    const VulkanTexture                    *baseTexture,
    const VulkanTexture                    *normalMap,
    const VulkanTexture                    *ormTexture,
    const VulkanTexture                    *emissiveTexture,
    const VulkanTexture                    *brdfTexture,
    const VulkanTexture                    *skyboxSpecular,
    const VulkanTexture                    *skyboxIrradiance,
    std::shared_ptr<VulkanGraphicsPipeline> pipeline,
    glm::vec3                               location,
    glm::quat                               rotation,
    glm::vec3                               scale
) {
    m_mesh            = mesh;
    m_baseTexture     = baseTexture;
    m_normalMap       = normalMap;
    m_ormTexture      = ormTexture;
    m_emissiveTexture = emissiveTexture;

    m_brdfTexture      = brdfTexture;
    m_skyboxSpecular   = skyboxSpecular;
    m_skyboxIrradiance = skyboxIrradiance;

    m_pipeline = pipeline;
    CreateDescriptorSets();

    m_originalLocation = location;
    m_location         = m_originalLocation;
    m_rotation         = rotation;
    m_scale            = scale;
    m_pitchYawRoll     = glm::eulerAngles(m_rotation);
    UpdateTransformation();
}

VulkanPrefab::VulkanPrefab(
    const VulkanMesh                       *mesh,
    const VulkanTexture                    *baseTexture,
    const VulkanTexture                    *normalMap,
    const VulkanTexture                    *ormTexture,
    const VulkanTexture                    *emissiveTexture,
    const VulkanTexture                    *brdfTexture,
    const VulkanTexture                    *skyboxSpecular,
    const VulkanTexture                    *skyboxIrradiance,
    std::shared_ptr<VulkanGraphicsPipeline> pipeline,
    glm::vec3                               location,
    glm::vec3                               pitchYawRoll,
    glm::vec3                               scale
) {
    m_mesh            = mesh;
    m_baseTexture     = baseTexture;
    m_normalMap       = normalMap;
    m_ormTexture      = ormTexture;
    m_emissiveTexture = emissiveTexture;

    m_brdfTexture      = brdfTexture;
    m_skyboxSpecular   = skyboxSpecular;
    m_skyboxIrradiance = skyboxIrradiance;

    m_pipeline = pipeline;
    CreateDescriptorSets();

    m_originalLocation = location;
    m_location         = m_originalLocation;
    m_pitchYawRoll     = pitchYawRoll;
    m_rotation         = glm::quat_cast(glm::yawPitchRoll(m_pitchYawRoll.y, m_pitchYawRoll.x, m_pitchYawRoll.z));
    UpdateTransformation();
}

void VulkanPrefab::Destroy() {
    if (!m_descriptorSets.empty()) {
        vkFreeDescriptorSets(
            VulkanState::GetInstance().GetDevice(),
            VulkanState::GetInstance().GetDescriptorPool(),
            m_descriptorSets.size(),
            m_descriptorSets.data()
        );
    }
    m_descriptorSets.clear();
    m_baseTexture     = nullptr;
    m_normalMap       = nullptr;
    m_ormTexture      = nullptr;
    m_emissiveTexture = nullptr;
    m_mesh            = nullptr;

    m_brdfTexture      = nullptr;
    m_skyboxSpecular   = nullptr;
    m_skyboxIrradiance = nullptr;
}

void VulkanPrefab::Swap(VulkanPrefab &other) noexcept {
    m_transformation   = other.m_transformation;
    m_location         = other.m_location;
    m_scale            = other.m_scale;
    m_rotation         = other.m_rotation;
    m_pitchYawRoll     = other.m_pitchYawRoll;
    m_originalLocation = other.m_originalLocation;
    std::swap(m_mesh, other.m_mesh);
    std::swap(m_pipeline, other.m_pipeline);
    std::swap(m_baseTexture, other.m_baseTexture);
    std::swap(m_normalMap, other.m_normalMap);
    std::swap(m_ormTexture, other.m_ormTexture);
    std::swap(m_emissiveTexture, other.m_emissiveTexture);

    std::swap(m_brdfTexture, other.m_brdfTexture);
    std::swap(m_skyboxSpecular, other.m_skyboxSpecular);
    std::swap(m_skyboxIrradiance, other.m_skyboxIrradiance);

    std::swap(m_descriptorSets, other.m_descriptorSets);
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

void VulkanPrefab::CreateDescriptorSets() {
    // The first set of a pipeline is always the uniform camera and light, which are handled in the VulkanState class
    for (size_t i = 1; i < m_pipeline->GetDescriptorSetLayouts().size(); i++) {
        VkDescriptorSet set = CreateDescriptorSet(m_pipeline->GetDescriptorSetLayouts()[i]);
        m_descriptorSets.push_back(set);
    }

    UpdatePBRDescriptorSet();
    UpdateIBLDescriptorSet();
}

VkDescriptorSet VulkanPrefab::CreateDescriptorSet(const VkDescriptorSetLayout &layout) {
    VkDescriptorSet set = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo infoSet{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = VulkanState::GetInstance().GetDescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts        = &layout
    };

    DEBUG_VK_ASSERT(vkAllocateDescriptorSets(VulkanState::GetInstance().GetDevice(), &infoSet, &set));

    return set;
}

void VulkanPrefab::UpdatePBRDescriptorSet() {
    std::vector<VkDescriptorImageInfo> infoImage{
        {.sampler = m_baseTexture->GetSampler(),         .imageView = m_baseTexture->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_normalMap->GetSampler(),           .imageView = m_normalMap->GetImageView(),   .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_ormTexture->GetSampler(),          .imageView = m_ormTexture->GetImageView(),  .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler     = m_emissiveTexture->GetSampler(),
         .imageView   = m_emissiveTexture->GetImageView(),
         .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL                                                                                            }
    };

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_descriptorSets[0],
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoImage.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = infoImage.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSet, 0, nullptr);
}

void VulkanPrefab::UpdateIBLDescriptorSet() {
    std::vector<VkDescriptorImageInfo> infoImage{
        {.sampler = m_brdfTexture->GetSampler(),          .imageView = m_brdfTexture->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler     = m_skyboxIrradiance->GetSampler(),
         .imageView   = m_skyboxIrradiance->GetImageView(),
         .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL                                                                                             },
        {.sampler     = m_skyboxSpecular->GetSampler(),
         .imageView   = m_skyboxSpecular->GetImageView(),
         .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL                                                                                             }
    };

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_descriptorSets[1],
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoImage.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = infoImage.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSet, 0, nullptr);
}

void VulkanPrefab::BindAndDraw(VkCommandBuffer cmdBuf) const {
    const VkDeviceSize offset = 0;

    if (!m_descriptorSets.empty()) {
        vkCmdBindDescriptorSets(
            cmdBuf,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipeline->GetLayout(),
            1,
            m_descriptorSets.size(),
            m_descriptorSets.data(),
            0,
            nullptr
        );
    }
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, &m_mesh->GetVertexBuffer(), &offset);
    vkCmdPushConstants(cmdBuf, m_pipeline->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(m_transformation), &m_transformation);
    vkCmdDraw(cmdBuf, m_mesh->GetVertexCount(), 1, 0, 0);
}
