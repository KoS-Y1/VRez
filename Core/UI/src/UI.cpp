#include "include/UI.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

#include <include/VulkanImage.h>
#include <include/VulkanState.h>
#include <include/MeshInstance.h>

#include <glm/gtc/type_ptr.hpp>

#include "glm/gtx/euler_angles.hpp"


UI::UI(SDL_Window *window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue,
       VkDescriptorPool descriptorPool)
{
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForVulkan(window);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

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
        .ApiVersion = VK_API_VERSION_1_4,
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

void UI::TransformationMenu(MeshInstance &instance)
{
    float min = -10.0f;
    float max = 10.0f;
    float step = 0.01f;
    float minAngle = -360.0f;
    float maxAngle = 360.0f;
    float angleStep = 1.0f;
    bool isReset = false;

    glm::vec3 location = instance.GetLocation();
    glm::vec3 scale = instance.GetScale();
    glm::vec3 rotation = glm::eulerAngles(instance.GetRotation());

    ImGui::Begin((instance.GetName() + " Transformation").c_str());
    ImGui::DragFloat3("Location(x, y, z)", glm::value_ptr(location), step, min, max);
    ImGui::DragFloat3("Rotate(pitch, yaw, roll)", glm::value_ptr(rotation), angleStep, minAngle, maxAngle);
    ImGui::DragFloat3("Scale(x, y, z)", glm::value_ptr(scale), step, min, max);
    if (ImGui::Button("Reset"))
    {
        isReset = true;
    }
    ImGui::End();

    instance.SetLocation(location);

    float yaw = glm::radians(rotation.y);
    float pitch = glm::radians(rotation.x);
    float roll = glm::radians(rotation.z);
    instance.SetRotation(glm::yawPitchRoll(yaw, pitch, roll));

    instance.SetScale(scale);


    if (isReset)
    {
        instance.Reset();
    }
}
