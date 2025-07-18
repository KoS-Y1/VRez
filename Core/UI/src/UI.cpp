#include "include/UI.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

#include <include/VulkanImage.h>
#include <include/VulkanState.h>
#include <include/MeshInstance.h>

#include <include/Camera.h>

#include <glm/gtc/type_ptr.hpp>

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

void UI::TransformationMenu(MeshInstance &instance, bool &uniformScale)
{
    float step = 0.1f;

    float angleStep = 1.0f;
    float minRoll = -180.0f;
    float maxRoll = 180.0f;
    float minPitch = -90.0f;
    float maxPitch = 90.0f;

    bool isReset = false;

    glm::vec3 location = instance.GetLocation();
    glm::vec3 scale = instance.GetScale();
    glm::vec3 pitchYawRoll = instance.GetPitchYawRoll();
    float yaw = glm::degrees(pitchYawRoll.y);
    float pitch = glm::degrees(pitchYawRoll.x);
    float roll = glm::degrees(pitchYawRoll.z);


    ImGui::Begin((instance.GetName() + " Transformation").c_str());
    ImGui::DragFloat3("Location(x, y, z)", glm::value_ptr(location), step);
    ImGui::Text("Rotation");
    ImGui::DragFloat("Yaw", &yaw, angleStep);
    ImGui::DragFloat("Pitch", &pitch, angleStep, minPitch, maxPitch);
    ImGui::DragFloat("Roll", &roll, angleStep, minRoll, maxRoll);
    ImGui::DragFloat3("Scale(x, y, z)", glm::value_ptr(scale), step);
    ImGui::Checkbox("Uniform Scale", &uniformScale);
    if (ImGui::Button("Reset"))
    {
        isReset = true;
    }
    ImGui::End();

    instance.SetLocation(location);

    instance.SetRotation(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));

    if (uniformScale)
    {
        if (scale.x != instance.GetScale().x)
        {
            scale = glm::vec3(scale.x, scale.x, scale.x);
        }
        else if (scale.y != instance.GetScale().y)
        {
            scale = glm::vec3(scale.y, scale.y, scale.y);
        }
        else if (scale.z != instance.GetScale().z)
        {
            scale = glm::vec3(scale.z, scale.z, scale.z);
        }
    }
    instance.SetScale(scale);

    if (isReset)
    {
        instance.Reset();
    }

}

void UI::CameraMenu()
{
    float step = 0.1f;

    float minFov = 1.0f;
    float maxFov = 45.0f;

    float angleStep = 1.0f;
    float minPitch = -90.0f;
    float maxPitch = 90.0f;

    bool isReset = false;

    glm::vec3 location = Camera::GetInstance().GetLocation();
    float fov = Camera::GetInstance().GetFOV();
    glm::vec3 pitchYawRoll = Camera::GetInstance().GetPitchYawRoll();
    float yaw = glm::degrees(pitchYawRoll.y);
    float pitch = glm::degrees(pitchYawRoll.x);

    ImGui::Begin("Camera");
    ImGui::DragFloat3("Location(x, y, z)", glm::value_ptr(location), step);
    ImGui::DragFloat("FOV", &fov, step, minFov, maxFov);
    ImGui::Text("Rotation");
    ImGui::DragFloat("Yaw", &yaw, angleStep);
    ImGui::DragFloat("Pitch", &pitch, angleStep, minPitch, maxPitch);
    if (ImGui::Button("Reset"))
    {
        isReset = true;
    }
    ImGui::End();

    Camera::GetInstance().SetLocation(location);
    Camera::GetInstance().SetFov(fov);
    Camera::GetInstance().SetRotation(glm::vec3(glm::radians(pitch), glm::radians(yaw), pitchYawRoll.z));

    if (isReset)
    {
        Camera::GetInstance().Reset();
    }
}
