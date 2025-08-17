#include "include/VulkanMaterial.h"

#include <include/VulkanState.h>
#include <include/VulkanTexture.h>

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

void VulkanMaterial::Bind(VkCommandBuffer cmdBuf, VkPipelineLayout layout, uint32_t firstSet) const {
    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, firstSet, 1, &m_descriptorSet, 0, nullptr);
}

void VulkanMaterial::CreateDescriptorSet(const VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo infoSet{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = VulkanState::GetInstance().GetDescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts        = &layout
    };
    DEBUG_VK_ASSERT(vkAllocateDescriptorSets(VulkanState::GetInstance().GetDevice(), &infoSet, &m_descriptorSet));

    std::vector<VkDescriptorImageInfo> infoImage{
        {.sampler = m_albedo->GetSampler(), .imageView = m_albedo->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_normal->GetSampler(), .imageView = m_normal->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_orm->GetSampler(),    .imageView = m_orm->GetImageView(),    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
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