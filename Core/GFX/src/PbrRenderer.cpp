#include "include/PbrRenderer.h"

#include <include/Camera.h>
#include <include/Descriptor.h>
#include <include/LightManager.h>
#include <include/MeshManager.h>
#include <include/PipelineManager.h>
#include <include/TextureManager.h>
#include <include/VulkanState.h>
#include <include/VulkanUtil.h>

PbrRenderer::PbrRenderer()
    : m_skybox("../Assets/Skybox/Skybox.png", PipelineManager::GetInstance().Load("skybox_gfx")->GetDescriptorSetLayouts()[descriptor::TEXTURE_SET]) {
    m_brdf       = TextureManager::GetInstance().Load("../Assets/Skybox/brdf_lut.png");
    m_irradiance = TextureManager::GetInstance().Load("../Assets/Skybox/irradiance.png");
    m_specular   = TextureManager::GetInstance().Load("../Assets/Skybox/specular.png");

    CreateImages();
    CreateBuffers();
    CreateDrawContent();
    CreateDescriptorSets();
    CreateRenderConfig();

    OneTimeUpdateDescriptorSets();
}

PbrRenderer::~PbrRenderer() {
    m_drawImage  = {};
    m_depthImage = {};

    m_drawContent.prefabs.clear();
    m_drawContent.screen = nullptr;

    m_brdf       = nullptr;
    m_irradiance = nullptr;
    m_specular   = nullptr;

    m_cameraBuffer = {};
    m_lightBuffer  = {};

    vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_uniformSet);
    vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_cameraSet);
    vkFreeDescriptorSets(VulkanState::GetInstance().GetDevice(), VulkanState::GetInstance().GetDescriptorPool(), 1, &m_iblSet);
}

void PbrRenderer::Render() {
    CameraData cameraData = Camera::GetInstance().Update();
    m_cameraBuffer.Upload(sizeof(CameraData), &cameraData);

    LightsData lightsData = LightManager::GetInstance().Update();
    m_lightBuffer.Upload(sizeof(LightsData), &lightsData);

    // Layout transition
    vk_util::CmdImageLayoutTransition(
        VulkanState::GetInstance().GetCommandBuffer(),
        m_depthImage.GetImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        0,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    );
    vk_util::CmdImageLayoutTransition(
        VulkanState::GetInstance().GetCommandBuffer(),
        m_drawImage.GetImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    );

    m_config.depthAttachments.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    m_config.drawAttachments.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // Shadow pass...

    m_gBufferPass.PreRender();
    m_gBufferPass.Render(
        m_config,
        {
            {m_cameraSet, descriptor::UNIFORM_SET}
    },
        m_drawContent,
        dynamic_cast<VulkanGraphicsPipeline *>(PipelineManager::GetInstance().Load("gbuffer_gfx"))
    );
    m_gBufferPass.PostRender();

    m_lightingPass.Render(
        m_config,
        {
            {m_uniformSet,                  descriptor::UNIFORM_SET},
            {m_gBufferPass.GetGBufferSet(), descriptor::TEXTURE_SET},
            {m_iblSet,                      descriptor::IBL_SET    }
    },
        m_drawContent,
        dynamic_cast<VulkanGraphicsPipeline *>(PipelineManager::GetInstance().Load("lighting_gfx"))
    );

    m_config.depthAttachments.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    m_config.drawAttachments.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    m_skybox.Render(
        m_config,
        {
            {m_cameraSet, descriptor::UNIFORM_SET},
    },
        m_drawContent,
        dynamic_cast<VulkanGraphicsPipeline *>(PipelineManager::GetInstance().Load("skybox_gfx"))
    );

    vk_util::CmdImageLayoutTransition(
        VulkanState::GetInstance().GetCommandBuffer(),
        m_drawImage.GetImage(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT
    );

    VulkanState::GetInstance().CopyToPresentImage(m_drawImage);
}

void PbrRenderer::CreateImages() {
    VulkanImage drawImg(
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        {VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight(), 1},
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    m_drawImage = std::move(drawImg);

    VulkanImage depthImg(
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        {VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight(), 1},
        VK_IMAGE_ASPECT_DEPTH_BIT
    );
    m_depthImage = std::move(depthImg);
}

void PbrRenderer::CreateBuffers() {
    VulkanBuffer camerabuffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    m_cameraBuffer = std::move(camerabuffer);

    VulkanBuffer lightBuffer(sizeof(LightsData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    m_lightBuffer = std::move(lightBuffer);
}

void PbrRenderer::CreateDrawContent() {
    m_drawContent.prefabs.emplace_back("../Assets/Models/BoomBox/BoomBox.json", glm::vec3(0.0f, 0.1f, 0.0f));
    m_drawContent.prefabs.emplace_back("../Assets/Models/Chessboard/Chessboard.json");
    m_drawContent.prefabs.emplace_back("../Assets/Models/Castle/Castle.json");

    m_drawContent.screen = MeshManager::GetInstance().Load("screen");
}

void PbrRenderer::CreateDescriptorSets() {
    m_uniformSet =
        vk_util::CreateDescriptorSet(PipelineManager::GetInstance().Load("lighting_gfx")->GetDescriptorSetLayouts()[descriptor::UNIFORM_SET]);

    m_cameraSet =
        vk_util::CreateDescriptorSet(PipelineManager::GetInstance().Load("gbuffer_gfx")->GetDescriptorSetLayouts()[descriptor::UNIFORM_SET]);

    m_iblSet = vk_util::CreateDescriptorSet(PipelineManager::GetInstance().Load("lighting_gfx")->GetDescriptorSetLayouts()[descriptor::IBL_SET]);
}

void PbrRenderer::CreateRenderConfig() {
    m_config.renderArea = {
        .offset = {0,                                     0                                     },
        .extent = {VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight()}
    };
    m_config.viewport = {
        .x        = 0.f,
        // Flip the view port
        .y        = static_cast<float>(VulkanState::GetInstance().GetHeight()),
        .width    = static_cast<float>(VulkanState::GetInstance().GetWidth()),
        // Flip the view port
        .height   = -static_cast<float>(VulkanState::GetInstance().GetHeight()),
        .minDepth = 0.f,
        .maxDepth = 1.f
    };

    VkClearValue colorClear{
        .color = {0.0f, 0.0f, 0.0f, 1.0f}
    };
    m_config.drawAttachments = vk_util::GetRenderingAttachmentInfo(
        m_drawImage.GetImageView(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        &colorClear,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_NONE,
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED
    );
    VkClearValue depthClear{
        .depthStencil = {.depth = 1.0f, .stencil = 0}
    };
    m_config.depthAttachments = vk_util::GetRenderingAttachmentInfo(
        m_depthImage.GetImageView(),
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        &depthClear,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_NONE,
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED
    );
}

void PbrRenderer::OneTimeUpdateDescriptorSets() {
    std::vector<VkDescriptorBufferInfo> infoBuffers{
        {.buffer = m_cameraBuffer.GetBuffer(), .offset = 0, .range = VK_WHOLE_SIZE},
        {.buffer = m_lightBuffer.GetBuffer(),  .offset = 0, .range = VK_WHOLE_SIZE}
    };

    VkWriteDescriptorSet writeSetUniform{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_uniformSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoBuffers.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = infoBuffers.data(),
        .pTexelBufferView = nullptr,
    };

    VkWriteDescriptorSet writeSetCamera{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_cameraSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = infoBuffers.data(),
        .pTexelBufferView = nullptr,
    };


    std::vector<VkDescriptorImageInfo> infoImages{
        {.sampler = m_brdf->GetSampler(),       .imageView = m_brdf->GetImageView(),       .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_irradiance->GetSampler(), .imageView = m_irradiance->GetImageView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {.sampler = m_specular->GetSampler(),   .imageView = m_specular->GetImageView(),   .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
    };

    VkWriteDescriptorSet writeSetIBL{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_iblSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoImages.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = infoImages.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSetUniform, 0, 0);
    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSetCamera, 0, 0);
    vkUpdateDescriptorSets(VulkanState::GetInstance().GetDevice(), 1, &writeSetIBL, 0, 0);
}
