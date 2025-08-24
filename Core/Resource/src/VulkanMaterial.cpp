#include "include/VulkanMaterial.h"

#include <include/VulkanState.h>
#include <include/VulkanTexture.h>

VulkanMaterial::VulkanMaterial(
    const VulkanTexture          *albedo,
    const VulkanTexture          *normal,
    const VulkanTexture          *orm,
    const VulkanTexture          *emissive,
    const VulkanGraphicsPipeline *pipeline
)
    : m_albedo(albedo)
    , m_normal(normal)
    , m_orm(orm)
    , m_emissive(emissive) {
    m_descriptorSet = vk_util::CreateDescriptorSet(pipeline->GetDescriptorSetLayouts()[Descriptor::TEXTURE_SET]);
    OneTimeUpdateDescriptorSets();
}

void VulkanMaterial::Swap(VulkanMaterial &other) noexcept {
    std::swap(m_albedo, other.m_albedo);
    std::swap(m_normal, other.m_normal);
    std::swap(m_orm, other.m_orm);
    std::swap(m_emissive, other.m_emissive);
    std::swap(m_descriptorSet, other.m_descriptorSet);
}

void VulkanMaterial::Destroy() {
    if (m_descriptorSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_descriptorSet);
    }

    m_descriptorSet = VK_NULL_HANDLE;
    m_albedo        = nullptr;
    m_normal        = nullptr;
    m_orm           = nullptr;
    m_emissive      = nullptr;
}

void VulkanMaterial::Bind(VkPipelineLayout layout, uint32_t firstSet) const {
    vkCmdBindDescriptorSets(
        VulkanState::GetInstance().GetCommandBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        layout,
        firstSet,
        1,
        &m_descriptorSet,
        0,
        nullptr
    );
}

void VulkanMaterial::OneTimeUpdateDescriptorSets() {
    std::vector<VkDescriptorImageInfo> infoImage{
        {.sampler = m_albedo->GetSampler(),   .imageView = m_albedo->GetImageView(),   .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_normal->GetSampler(),   .imageView = m_normal->GetImageView(),   .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_orm->GetSampler(),      .imageView = m_orm->GetImageView(),      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_emissive->GetSampler(), .imageView = m_emissive->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
    };

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_descriptorSet,
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