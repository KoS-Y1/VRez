#include "include/VulkanSkybox.h"

#include <include/Camera.h>
#include <include/JsonInput.h>
#include <include/MeshManager.h>
#include <include/PipelineManager.h>
#include <include/TextureManager.h>
#include <include/VulkanGraphicsPipeline.h>
#include <include/VulkanMesh.h>
#include <include/VulkanState.h>
#include <include/VulkanTexture.h>
#include <include/DescriptorSets.h>

VulkanSkybox::VulkanSkybox(const std::string &jsonFile, std::vector<VkDescriptorSetLayout> layouts) {
    file_system::SkyboxConfig config = file_system::SkyboxConfig(jsonFile);

    m_emissive   = TextureManager::GetInstance().Load(config.emissive);
    m_specular   = TextureManager::GetInstance().Load(config.specular);
    m_irradiance = TextureManager::GetInstance().Load(config.irradiance);
    m_brdf       = TextureManager::GetInstance().Load(config.brdf);

    m_mesh = MeshManager::GetInstance().Load("skybox");

    DEBUG_ASSERT(layouts.size() >= 2);
    m_cameraSet  = CreateDescriptorSet(layouts[0]);
    m_textureSet = CreateDescriptorSet(layouts[1]);

    // Note that ibl texture set is always at set 2
    m_iblSet     = CreateDescriptorSet(
        dynamic_cast<VulkanGraphicsPipeline *>(PipelineManager::GetInstance().Load("basic_gfx"))->GetDescriptorSetLayouts()[IBL_SET]
    );


    OneTimeUpdateDescriptorSets();
}

void VulkanSkybox::Swap(VulkanSkybox &other) noexcept {
    std::swap(m_cameraSet, other.m_cameraSet);
    std::swap(m_textureSet, other.m_textureSet);
    std::swap(m_iblSet, other.m_iblSet);
    std::swap(m_mesh, other.m_mesh);
    std::swap(m_emissive, other.m_emissive);
    std::swap(m_specular, other.m_specular);
    std::swap(m_irradiance, other.m_irradiance);
    std::swap(m_brdf, other.m_brdf);
}

void VulkanSkybox::Destroy() {
    if (m_cameraSet != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_cameraSet);
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_textureSet);
        vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_iblSet);
    }

    m_cameraSet  = VK_NULL_HANDLE;
    m_textureSet = VK_NULL_HANDLE;
    m_iblSet     = VK_NULL_HANDLE;
    m_emissive   = nullptr;
    m_specular   = nullptr;
    m_irradiance = nullptr;
    m_brdf       = nullptr;
}

void VulkanSkybox::BindAndDraw(VkCommandBuffer cmdBuf, const VulkanGraphicsPipeline *pipeline) {
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 0, 1, &m_cameraSet, 0, nullptr);
    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 1, 1, &m_textureSet, 0, nullptr);

    m_mesh->BindAndDraw(cmdBuf);
}

VkDescriptorSet VulkanSkybox::CreateDescriptorSet(VkDescriptorSetLayout layout) {
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

void VulkanSkybox::OneTimeUpdateDescriptorSets() {
    VkDescriptorBufferInfo infoBuffer{.buffer = Camera::GetInstance().GetBuffer(), .offset = 0, .range = VK_WHOLE_SIZE};

    VkDescriptorImageInfo infoImage{
        .sampler     = m_emissive->GetSampler(),
        .imageView   = m_emissive->GetImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet writeSetBuffer{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_cameraSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = &infoBuffer,
        .pTexelBufferView = nullptr,
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

    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSetBuffer, 0, nullptr);
    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSetImage, 0, nullptr);

    std::vector<VkDescriptorImageInfo> infoIblImages{
        {.sampler = m_brdf->GetSampler(),       .imageView = m_brdf->GetImageView(),       .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_irradiance->GetSampler(), .imageView = m_irradiance->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_specular->GetSampler(),   .imageView = m_specular->GetImageView(),   .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
    };

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_iblSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoIblImages.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = infoIblImages.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };
    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSet, 0, nullptr);
}