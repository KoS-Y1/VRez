#include "include/ShadowPass.h"

#include <include/Descriptor.h>
#include <include/Camera.h>
#include <include/PbrRenderer.h>
#include <include/PipelineManager.h>
#include <include/VulkanState.h>
#include <include/VulkanUtil.h>

ShadowPass::ShadowPass() {
    CreateShadowMapImage();
    CreateCSMSet();
}

ShadowPass::~ShadowPass() {
    if (m_csmSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_csmSet);
        vkDestroySampler(VulkanState::GetInstance().GetDevice(), m_sampler, nullptr);
    }

    m_csmSet  = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;

    m_shadowMap        = {};
    m_shadowAttachment = {};
}

void ShadowPass::CreateRenderingInfo(const RenderingConfig &config) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, nullptr, &m_shadowAttachment, CASCADES_NUM);
}

void ShadowPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    for (const auto &prefab: content.prefabs) {
        prefab.BindAndDrawMesh(layout);
    }
}

void ShadowPass::PreRender() {
    vk_util::CmdImageLayoutTransition(
        VulkanState::GetInstance().GetCommandBuffer(),
        m_shadowMap.GetImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        0,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        1,
        CASCADES_NUM
    );
}

void ShadowPass::PostRender() {
    vk_util::CmdImageLayoutTransition(
        VulkanState::GetInstance().GetCommandBuffer(),
        m_shadowMap.GetImage(),
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        1,
        CASCADES_NUM
    );
}

void ShadowPass::CreateShadowMapImage() {
    VkClearValue colorClear = {
        .depthStencil = {.depth = 1.0f, .stencil = 0}
    };

    VulkanImage shadowMap(
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        {VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight(), 1},
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_SAMPLE_COUNT_1_BIT,
        1,
        CASCADES_NUM
    );

    m_shadowMap = std::move(shadowMap);

    m_shadowAttachment = vk_util::GetRenderingAttachmentInfo(
        m_shadowMap.GetImageView(),
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        &colorClear,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_NONE,
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED
    );
}

void ShadowPass::CreateCSMSet() {
    {
        VkSamplerCreateInfo infoSampler = {
            .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = 0,
            .magFilter               = VK_FILTER_LINEAR,
            .minFilter               = VK_FILTER_LINEAR,
            .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias              = 0.0f,
            .anisotropyEnable        = VK_FALSE,
            .maxAnisotropy           = 1.0f,
            .compareEnable           = VK_TRUE,
            .compareOp               = VK_COMPARE_OP_LESS_OR_EQUAL,
            .minLod                  = 0.0f,
            .maxLod                  = 1.0f,
            .borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE, // Always normalized
        };

        vkCreateSampler(VulkanState::GetInstance().GetDevice(), &infoSampler, nullptr, &m_sampler);
    }

    m_csmSet = vk_util::CreateDescriptorSet(PipelineManager::GetInstance().Load("lighting_gfx")->GetDescriptorSetLayouts()[descriptor::CSM_SET]);

    VkDescriptorImageInfo infoImage{
        .sampler     = m_sampler,
        .imageView   = m_shadowMap.GetImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_csmSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = &infoImage,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSet, 0, 0);
}