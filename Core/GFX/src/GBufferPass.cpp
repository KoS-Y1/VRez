#include "include/GBufferPass.h"

#include <include/Descriptor.h>
#include <include/PbrRenderer.h>
#include <include/PipelineManager.h>
#include <include/VulkanState.h>
#include <include/VulkanUtil.h>

GBufferPass::GBufferPass() {
    CreateGBufferImages();
    CreateGBufferSet();
}

GBufferPass::~GBufferPass() {
    if (m_gBufferSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_gBufferSet);
        vkDestroySampler(VulkanState::GetInstance().GetDevice(), m_sampler, nullptr);
    }
    m_gBufferSet = VK_NULL_HANDLE;
    m_gBufferImages.clear();
    m_gBufferAttachments.clear();
}

void GBufferPass::CreateRenderingInfo(const RenderingConfig &config) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, m_gBufferAttachments, &config.depthAttachments);
}

void GBufferPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    for (const auto &prefab: content.prefabs) {
        prefab.BindAndDraw(layout);
    }
}

void GBufferPass::PreRender() {
    // Layout transition to color attachment
    for (auto &image: m_gBufferImages) {
        vk_util::CmdImageLayoutTransition(
            VulkanState::GetInstance().GetCommandBuffer(),
            image.GetImage(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        );
    }
}

void GBufferPass::PostRender() {
    // Layout transition to sampled image in lighting pass
    for (auto &image: m_gBufferImages) {
        vk_util::CmdImageLayoutTransition(
            VulkanState::GetInstance().GetCommandBuffer(),
            image.GetImage(),
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT
        );
    }
}

void GBufferPass::CreateGBufferSet() {
    VkSamplerCreateInfo infoSampler = {
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .magFilter               = VK_FILTER_LINEAR,
        .minFilter               = VK_FILTER_LINEAR,
        .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = VK_FALSE,
        .maxAnisotropy           = 1.0f,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 0.0f,
        .maxLod                  = 1.0f,
        .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE, // Always normalized
    };

    vkCreateSampler(VulkanState::GetInstance().GetDevice(), &infoSampler, nullptr, &m_sampler);

    m_gBufferSet =
        vk_util::CreateDescriptorSet(PipelineManager::GetInstance().Load("lighting_gfx")->GetDescriptorSetLayouts()[descriptor::TEXTURE_SET]);

    std::vector<VkDescriptorImageInfo> infoImages;
    for (const auto &image: m_gBufferImages) {
        infoImages.emplace_back(m_sampler, image.GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_gBufferSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoImages.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = infoImages.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };
    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSet, 0, 0);
}

void GBufferPass::CreateGBufferImages() {
    constexpr size_t gBufferImageCount = 4;

    VkExtent3D   extent{VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight(), 1};
    VkClearValue colorClear{
        .color = {0.0f, 0.0f, 0.0f, 1.0f}
    };

    for (size_t i = 0; i < gBufferImageCount; i++) {
        m_gBufferImages.emplace_back(
            VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            extent,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
        m_gBufferAttachments.push_back(
            vk_util::GetRenderingAttachmentInfo(
                m_gBufferImages[i].GetImageView(),
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                &colorClear,
                VK_ATTACHMENT_LOAD_OP_CLEAR,
                VK_ATTACHMENT_STORE_OP_STORE,
                VK_RESOLVE_MODE_NONE,
                VK_NULL_HANDLE,
                VK_IMAGE_LAYOUT_UNDEFINED
            )
        );
    }
}