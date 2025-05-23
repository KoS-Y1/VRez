#include "include/UI.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

#include <include/VulkanImage.h>


UI::UI(SDL_Window *window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkDescriptorPool descriptorPool)
{
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForVulkan(window);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    VkFormat colorFormat = IMG_FORMAT;

    VkPipelineRenderingCreateInfoKHR infoRendering
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    ImGui_ImplVulkan_InitInfo infoInit
    {
        .ApiVersion =  VK_API_VERSION_1_4,
        .Instance = instance,
        .PhysicalDevice = physicalDevice,
        .Device = device,
        .QueueFamily = 0,
        .Queue = queue,
        .DescriptorPool = descriptorPool,
        .RenderPass = VK_NULL_HANDLE,
        .MinImageCount = 3,
        .ImageCount = 3,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo = infoRendering,
    };

    ImGui_ImplVulkan_Init(&infoInit);
    ImGui_ImplVulkan_CreateFontsTexture();
}
