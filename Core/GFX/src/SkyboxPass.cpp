#include "include/SkyboxPass.h"

#include "include/VulkanUtil.h"

#include <include/Descriptor.h>
#include <include/JsonInput.h>
#include <include/MeshManager.h>
#include <include/TextureManager.h>
#include <include/VulkanMesh.h>
#include <include/VulkanState.h>
#include <include/VulkanTexture.h>

SkyboxPass::SkyboxPass(const std::string &file, VkDescriptorSetLayout layout) {
    m_emissive = TextureManager::GetInstance().Load(file);

    m_mesh = MeshManager::GetInstance().Load("skybox");

    m_textureSet = vk_util::CreateDescriptorSet(layout);

    OneTimeUpdateDescriptorSets();
}

SkyboxPass::~SkyboxPass() {
    if (m_textureSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_textureSet);
    }

    m_textureSet = VK_NULL_HANDLE;
    m_emissive   = nullptr;
}

void SkyboxPass::CreateRenderingInfo(const RenderingConfig &config) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, &config.drawAttachments, &config.depthAttachments);
}

void SkyboxPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    vkCmdBindDescriptorSets(
        VulkanState::GetInstance().GetCommandBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        layout,
        descriptor::TEXTURE_SET,
        1,
        &m_textureSet,
        0,
        nullptr
    );
    m_mesh->BindAndDraw();
}

void SkyboxPass::OneTimeUpdateDescriptorSets() {
    VkDescriptorImageInfo infoImage{
        .sampler     = m_emissive->GetSampler(),
        .imageView   = m_emissive->GetImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet writeSetImage{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_textureSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = &infoImage,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };
    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSetImage, 0, nullptr);
}
