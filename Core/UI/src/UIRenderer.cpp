#include "include/UIRenderer.h"

#include <imgui_impl_vulkan.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <include/DescriptorSets.h>
#include <include/VulkanState.h>
#include <include/Window.h>
#include <include/VulkanUtil.h>

UIRenderer::UIRenderer() {
    CreateDescriptorPool();

    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForVulkan(Window::GetInstance().GetSDLWindow());
    ImGuiIO &io     = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    VkFormat colorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

    VkPipelineRenderingCreateInfoKHR infoRendering{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .pNext                   = nullptr,
        .viewMask                = 0,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    ImGui_ImplVulkan_InitInfo infoInit{
        .ApiVersion                  = VK_API_VERSION_1_4,
        .Instance                    = VulkanState::GetInstance().GetVkInstance(),
        .PhysicalDevice              = VulkanState::GetInstance().GetPhysicalDevice(),
        .Device                      = VulkanState::GetInstance().GetDevice(),
        .QueueFamily                 = 0,
        .Queue                       = VulkanState::GetInstance().GetQueue(),
        .DescriptorPool              = m_imguiDescriptorPool,
        .RenderPass                  = VK_NULL_HANDLE,
        .MinImageCount               = 3,
        .ImageCount                  = 3,
        .MSAASamples                 = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering         = true,
        .PipelineRenderingCreateInfo = infoRendering,
    };

    ImGui_ImplVulkan_Init(&infoInit);
    ImGui_ImplVulkan_CreateFontsTexture();

    Enqueue([this]() { m_ui.CameraWindow(); });
    Enqueue([this]() { m_ui.LightsWindow(); });
}

void UIRenderer::Destroy() {
    vkDestroyDescriptorPool(VulkanState::GetInstance().GetDevice(), m_imguiDescriptorPool, nullptr);
    m_uniformScales.clear();
    m_uiQueue.clear();
}


void UIRenderer::Render() {
    // Layout transition for imgui draw
    vk_util::CmdImageLayoutTransition(
        VulkanState::GetInstance().GetCommandBuffer(),
        VulkanState::GetInstance().GetPresentImage(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );

    VkRect2D renderAreas{
        .offset = {0,                                     0                                     },
        .extent = {VulkanState::GetInstance().GetWidth(), VulkanState::GetInstance().GetHeight()}
    };
    VkRenderingAttachmentInfo infoColorAttachment = vk_util::GetRenderingAttachmentInfo(
        VulkanState::GetInstance().GetPresentImageView(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        nullptr,
        VK_ATTACHMENT_LOAD_OP_LOAD,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_NONE,
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED
    );
    VkRenderingInfo infoRendering = vk_util::GetRenderingInfo(renderAreas, &infoColorAttachment, nullptr);

    vkCmdBeginRendering(VulkanState::GetInstance().GetCommandBuffer(), &infoRendering);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VulkanState::GetInstance().GetCommandBuffer());

    vkCmdEndRendering(VulkanState::GetInstance().GetCommandBuffer());
}

void UIRenderer::AddPrefabWindow(VulkanPrefab &prefab, size_t prefabIndex) {
    m_uniformScales.push_back(false);
    Enqueue([this, &prefab, prefabIndex]() {
        bool uniformScale = m_uniformScales[prefabIndex];
        m_ui.TransformationWindow(prefab, uniformScale, prefabIndex);
        m_uniformScales[prefabIndex] = uniformScale;
    });
}

void UIRenderer::Present() {
    for (auto it = m_uiQueue.begin(); it != m_uiQueue.end(); ++it) {
        (*it)();
    }
}

void UIRenderer::CreateDescriptorPool() {
    VkDescriptorPoolSize poolSizes[]{
        {VK_DESCRIPTOR_TYPE_SAMPLER,                MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, MAX_DESCRIPTOR_SET_COUNT},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       MAX_DESCRIPTOR_SET_COUNT}
    };

    VkDescriptorPoolCreateInfo infoPool{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets       = MAX_DESCRIPTOR_SET_COUNT,
        .poolSizeCount = static_cast<uint32_t>(std::size(poolSizes)),
        .pPoolSizes    = poolSizes
    };

    DEBUG_VK_ASSERT(vkCreateDescriptorPool(VulkanState::GetInstance().GetDevice(), &infoPool, nullptr, &m_imguiDescriptorPool));
}
