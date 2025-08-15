#include "include/VulkanState.h"

#include "include/ThreadPool.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>
#include <imgui_impl_vulkan.h>
#include <include/VulkanUtil.h>

#include <include/Camera.h>
#include <include/LightManager.h>
#include <include/MeshManager.h>
#include <include/TextureManager.h>
#include <include/ThreadPool.h>
#include <include/VertexFormats.h>
#include <include/VulkanComputePipeline.h>
#include <include/VulkanGraphicsPipeline.h>
#include <include/VulkanPrefab.h>
#include <include/Window.h>

void VulkanState::Init() {
    m_window = Window::GetInstance().GetSDLWindow();
    m_width  = Window::GetInstance().GetWidth();
    m_height = Window::GetInstance().GetHeight();

    CreateInstance();
    CreatePhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSurface(Window::GetInstance().GetSDLWindow());
    CreateSwapchain(Window::GetInstance().GetWidth(),  Window::GetInstance().GetHeight());

    m_renderFence      = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
    m_immediateFence   = CreateFence(0);
    m_renderSemaphore  = CreateSemaphore();
    m_presentSemaphore = CreateSemaphore();

    CreateDescriptorPool();
    CreatePipelines();

    MeshManager::GetInstance().Init();
    TextureManager::GetInstance().Init();
    // Wait for all meshes and textures are loaded
    ThreadPool::GetInstance().WaitIdle();

    CreateRenderObjects();

    LoadMeshes();
}

void VulkanState::Destroy() {
    if (m_device == VK_NULL_HANDLE) {
        return;
    }

    WaitIdle();

    vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_uniformDescriptorSet);
    vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_uniformViewDescriptorSet);
    vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_skyboxDescriptorSet);

    LightManager::GetInstance().Destroy();
    Camera::GetInstance().Destroy();

    for (size_t i = 0; i < m_computePipelines.size(); ++i) {
        m_computePipelines[i]->Destroy();
    }

    for (size_t i = 0; i < m_graphicsPipelines.size(); ++i) {
        m_graphicsPipelines[i]->Destroy();
    }

    m_skyboxPipeline->Destroy();

    m_drawImage.Destroy();
    m_depthImage.Destroy();
    m_msaaColorImage.Destroy();

    for (size_t i = 0; i < m_swapchain.count; i++) {
        vkDestroyImageView(m_device, m_swapchain.views[i], nullptr);
    }

    for (auto &mesh: m_meshInstances) {
        mesh.Destroy();
    }
    m_meshInstances.clear();

    m_skybox.Destroy();


    MeshManager::GetInstance().Destroy();
    TextureManager::GetInstance().Destroy();

    m_deletionQueue.Flush();

    m_device = VK_NULL_HANDLE;
}

void VulkanState::CreateInstance() {
    VkApplicationInfo infoApp{
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = "VulkanRayTracerApp",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName        = "VulkanRayTracerEngine",
        .apiVersion         = VK_API_VERSION_1_4,
    };

    // Enable validation layer
    std::vector<const char *> layers{
        "VK_LAYER_KHRONOS_validation",
    };

    uint32_t                  sdlExtensionCount = 0;
    const char * const       *sdlExtensions     = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    std::vector<const char *> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    VkInstanceCreateInfo infoInstance{
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .pApplicationInfo        = &infoApp,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    DEBUG_VK_ASSERT(vkCreateInstance(&infoInstance, nullptr, &m_instance));

    m_deletionQueue.PushFunction([&]() { vkDestroyInstance(m_instance, nullptr); });
}

void VulkanState::CreatePhysicalDevice() {
    // Query the num of physical devices
    uint32_t physicalDeviceCount = 0;
    DEBUG_VK_ASSERT(vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr));
    DEBUG_ASSERT(physicalDeviceCount > 0);
    // Query all physical devices
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    DEBUG_VK_ASSERT(vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data()));

    // Get the physical device with the max api version(usually the best one)
    uint32_t maxApiVersion = 0;
    for (VkPhysicalDevice p: physicalDevices) {
        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(p, &properties);

        if (properties.apiVersion > maxApiVersion) {
            m_physicalDevice = p;
            maxApiVersion    = properties.apiVersion;
        }
    }

    VkPhysicalDeviceProperties properties = {0};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
    VkSampleCountFlags sampleCounts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

    // Set sample count
    if (sampleCounts & VK_SAMPLE_COUNT_8_BIT) {
        m_sampleCount = VK_SAMPLE_COUNT_8_BIT;
    } else if (sampleCounts & VK_SAMPLE_COUNT_4_BIT) {
        m_sampleCount = VK_SAMPLE_COUNT_4_BIT;
    } else if (sampleCounts & VK_SAMPLE_COUNT_2_BIT) {
        m_sampleCount = VK_SAMPLE_COUNT_2_BIT;
    } else if (sampleCounts & VK_SAMPLE_COUNT_1_BIT) {
        m_sampleCount = VK_SAMPLE_COUNT_1_BIT;
    }

    SDL_Log(
        "Selected physical device: %s %d.%d.%d",
        properties.deviceName,
        VK_API_VERSION_MAJOR(properties.apiVersion),
        VK_API_VERSION_MINOR(properties.apiVersion),
        VK_API_VERSION_PATCH(properties.apiVersion)
    );
}

void VulkanState::CreateDevice() {
    float                   priority = 1.0f;
    // Only use one queue, and use the first one
    VkDeviceQueueCreateInfo infoQueue{
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = 0,
        .queueFamilyIndex = 0,
        .queueCount       = 1,
        .pQueuePriorities = &priority,
    };

    // Enable swapchain extension for presenting on screen
    std::vector<const char *> extensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_create_renderpass2",
        "VK_KHR_depth_stencil_resolve",
        "VK_KHR_dynamic_rendering",
        "VK_KHR_depth_stencil_resolve"
    };

    VkPhysicalDeviceFeatures feature{.sampleRateShading = VK_TRUE};

    VkPhysicalDeviceVulkan13Features feature13{
        .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext            = nullptr,
        .dynamicRendering = VK_TRUE,
    };

    VkDeviceCreateInfo infoDevice{
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = &feature13,
        .flags                   = 0,
        .queueCreateInfoCount    = 1,
        .pQueueCreateInfos       = &infoQueue,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nullptr,
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = &feature,
    };
    DEBUG_VK_ASSERT(vkCreateDevice(m_physicalDevice, &infoDevice, nullptr, &m_device));

    // Get queue
    vkGetDeviceQueue(m_device, 0, 0, &m_queue);

    m_deletionQueue.PushFunction([&]() { vkDestroyDevice(m_device, nullptr); });
}

void VulkanState::CreateCommandPool() {
    VkCommandPoolCreateInfo infoCommandPool{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };
    DEBUG_VK_ASSERT(vkCreateCommandPool(m_device, &infoCommandPool, nullptr, &m_commandPool));

    m_deletionQueue.PushFunction([&]() { vkDestroyCommandPool(m_device, m_commandPool, nullptr); });
}

void VulkanState::CreateSurface(SDL_Window *window) {
    DEBUG_ASSERT(SDL_Vulkan_CreateSurface(window, m_instance, nullptr, &m_surface));

    m_deletionQueue.PushFunction([&]() { vkDestroySurfaceKHR(m_instance, m_surface, nullptr); });
}

void VulkanState::CreateSwapchain(uint32_t width, uint32_t height) {
    VkSwapchainCreateInfoKHR infoSwapchain{
        .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext            = nullptr,
        .flags            = 0,
        .surface          = m_surface,
        .minImageCount    = MIN_SWAPCHAIN_IMG_COUNT,
        .imageFormat      = COLOR_IMG_FORMAT,
        .imageColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent      = {width, height},
        .imageArrayLayers = 1,
        .imageUsage =
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = VK_PRESENT_MODE_FIFO_KHR,
        .clipped               = false,
        .oldSwapchain          = nullptr,
    };
    DEBUG_VK_ASSERT(vkCreateSwapchainKHR(m_device, &infoSwapchain, nullptr, &m_swapchain.swapchain));

    // Get swapchain images
    DEBUG_VK_ASSERT(vkGetSwapchainImagesKHR(m_device, m_swapchain.swapchain, &m_swapchain.count, nullptr));
    DEBUG_VK_ASSERT(vkGetSwapchainImagesKHR(m_device, m_swapchain.swapchain, &m_swapchain.count, m_swapchain.images));

    // Create image view for each swapchain image
    {
        VkImageViewCreateInfo infoView{
            .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = 0,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format   = COLOR_IMG_FORMAT,
            .components =
                {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = vk_util::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT)
        };

        for (size_t i = 0; i < m_swapchain.count; i++) {
            infoView.image = m_swapchain.images[i];
            DEBUG_VK_ASSERT(vkCreateImageView(m_device, &infoView, nullptr, &m_swapchain.views[i]));
        }
    }

    // Init drawImage that swapchain images copy from
    VulkanImage drawImg(
        COLOR_IMG_FORMAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        {m_width, m_height, 1},
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    m_drawImage = std::move(drawImg);

    // Init depth image for depth testing
    VulkanImage depthImg(
        DEPTH_IMG_FORMAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        {m_width, m_height, 1},
        VK_IMAGE_ASPECT_DEPTH_BIT,
        m_sampleCount
    );
    m_depthImage = std::move(depthImg);

    // Init msaa images for anti aliasing
    VulkanImage msaaColorImage(
        COLOR_IMG_FORMAT,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        {m_width, m_height, 1},
        VK_IMAGE_ASPECT_COLOR_BIT,
        m_sampleCount
    );
    m_msaaColorImage = std::move(msaaColorImage);

    m_deletionQueue.PushFunction([&]() { vkDestroySwapchainKHR(m_device, m_swapchain.swapchain, nullptr); });
}

VkSemaphore VulkanState::CreateSemaphore() {
    VkSemaphore           semaphore = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    DEBUG_VK_ASSERT(vkCreateSemaphore(m_device, &createInfo, nullptr, &semaphore));

    // Pass the copy of the object since there's no such class memeber
    m_deletionQueue.PushFunction([this, semaphore]() { vkDestroySemaphore(m_device, semaphore, nullptr); });

    return semaphore;
}

VkFence VulkanState::CreateFence(const VkFenceCreateFlags flag) {
    VkFence           fence = VK_NULL_HANDLE;
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flag,
    };
    DEBUG_VK_ASSERT(vkCreateFence(m_device, &createInfo, nullptr, &fence));

    m_deletionQueue.PushFunction([this, fence]() { vkDestroyFence(m_device, fence, nullptr); });

    return fence;
}

void VulkanState::CreateCommandBuffer() {
    VkCommandBufferAllocateInfo infoCmdBuffer{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    DEBUG_VK_ASSERT(vkAllocateCommandBuffers(m_device, &infoCmdBuffer, &m_cmdBuf));
    DEBUG_VK_ASSERT(vkAllocateCommandBuffers(m_device, &infoCmdBuffer, &m_immediateCmdBuf));

    m_deletionQueue.PushFunction([&]() { vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_cmdBuf); });
    m_deletionQueue.PushFunction([&]() { vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_immediateCmdBuf); });
}

void VulkanState::CreateDescriptorPool() {
    VkDescriptorPoolSize poolSize{
        .type            = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
    };

    // TODO: This is just for testing for now
    // Needs to update in the future
    VkDescriptorPoolCreateInfo infoPool{
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets       = MAX_DESCRIPTOR_SET_COUNT,
        .poolSizeCount = 1,
        .pPoolSizes    = &poolSize
    };

    DEBUG_VK_ASSERT(vkCreateDescriptorPool(m_device, &infoPool, nullptr, &m_descriptorPool));

    m_deletionQueue.PushFunction([&]() { vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr); });

    // Create descriptor pool for ImGui
    VkDescriptorPoolSize poolSizes[]{
        {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
    };

    infoPool.maxSets       = 1000;
    infoPool.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    infoPool.pPoolSizes    = poolSizes;

    DEBUG_VK_ASSERT(vkCreateDescriptorPool(m_device, &infoPool, nullptr, &m_imguiDescriptorPool));
    m_deletionQueue.PushFunction([&]() { vkDestroyDescriptorPool(m_device, m_imguiDescriptorPool, nullptr); });
}

VkDescriptorSet VulkanState::CreateDescriptorSet(const VkDescriptorSetLayout layout) {
    VkDescriptorSet set = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo infoSet{
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext              = nullptr,
        .descriptorPool     = m_descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &layout
    };

    DEBUG_VK_ASSERT(vkAllocateDescriptorSets(m_device, &infoSet, &set));

    return set;
}

void VulkanState::WaitIdle() {
    DEBUG_VK_ASSERT(vkDeviceWaitIdle(m_device));
}

void VulkanState::Present() {
    // Reset fence and command buffer
    WaitAndResetFence(m_renderFence);
    DEBUG_VK_ASSERT(vkResetCommandBuffer(m_cmdBuf, 0));

    // Acquire next image in the swapchain for presenting
    uint32_t imageIndex = 0;
    DEBUG_VK_ASSERT(vkAcquireNextImageKHR(m_device, m_swapchain.swapchain, POINT_ONE_SECOND, m_presentSemaphore, nullptr, &imageIndex));

    BeginCommandBuffer(m_cmdBuf, 0);

    // Layout transition so that we can clear image color
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_drawImage.GetImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );

    // Layout transition for drawing
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_drawImage.GetImage(),
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    );

    // Layout transition for depth testing
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_depthImage.GetImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        0,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    );

    // Layout transition for msaa
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_msaaColorImage.GetImage(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    );
    // Draw
    Draw();

    // Layout transition for copying image
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_drawImage.GetImage(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_swapchain.images[imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );

    // Copy draw image to the current swapchain image
    vk_util::CopyImageToImage(
        m_cmdBuf,
        m_drawImage.GetImage(),
        m_swapchain.images[imageIndex],
        m_drawImage.GetExtent(),
        {m_width, m_height, 1},
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    // Layout transition for imgui draw
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_swapchain.images[imageIndex],
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );
    DrawImgui(m_swapchain.views[imageIndex]);

    // Layout transition for presenting
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_swapchain.images[imageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        0
    );

    EndAndSubmitCommandBuffer(m_cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_renderFence, m_presentSemaphore, m_renderSemaphore);

    QueuePresent(m_renderSemaphore, imageIndex);
}

void VulkanState::CreatePipelines() {
    std::vector<std::pair<std::vector<std::string>, PipelineType>> shaderSources{
        {{"../Assets/Shaders/BasicShader/basic.vert", "../Assets/Shaders/BasicShader/basic.frag"}, PipelineType::Graphics}
    };

    GraphicsPipelineConfig graphicsConfig;
    graphicsConfig.infoVertex           = VertexPNTT::GetVertexInputStateCreateInfo();
    graphicsConfig.colorFormats         = std::vector<VkFormat>{COLOR_IMG_FORMAT};
    graphicsConfig.depthTestEnable      = VK_TRUE;
    graphicsConfig.depthWriteEnable     = VK_TRUE;
    graphicsConfig.depthFormat          = DEPTH_IMG_FORMAT;
    graphicsConfig.depthCompareOp       = VK_COMPARE_OP_LESS_OR_EQUAL;
    graphicsConfig.rasterizationSamples = m_sampleCount;

    for (const auto &source: shaderSources) {
        m_graphicsPipelines.emplace_back(std::make_shared<VulkanGraphicsPipeline>(source.first, graphicsConfig));
    }

    std::vector<std::string> skyboxPaths{
        "../Assets/Shaders/Skybox/Skybox.vert",
        "../Assets/Shaders/Skybox/Skybox.frag",
    };
    GraphicsPipelineConfig skyboxConfig;
    skyboxConfig.infoVertex           = VertexP::GetVertexInputStateCreateInfo();
    skyboxConfig.colorFormats         = std::vector<VkFormat>{COLOR_IMG_FORMAT};
    skyboxConfig.topology             = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    skyboxConfig.depthTestEnable      = VK_TRUE;
    skyboxConfig.depthWriteEnable     = VK_TRUE;
    skyboxConfig.depthFormat          = DEPTH_IMG_FORMAT;
    skyboxConfig.depthCompareOp       = VK_COMPARE_OP_LESS_OR_EQUAL;
    skyboxConfig.rasterizationSamples = m_sampleCount;
    skyboxConfig.cullMode             = VK_CULL_MODE_NONE;
    m_skyboxPipeline                  = std::make_unique<VulkanGraphicsPipeline>(skyboxPaths, skyboxConfig);
}

void VulkanState::CreateSkybox() {
    std::vector<VertexP> vertices{
        VertexP(glm::vec3(-1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(-1.0f, -1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, -1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, -1.0f, -1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, -1.0f)),
        VertexP(glm::vec3(-1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(-1.0f, 1.0f, -1.0f)),
        VertexP(glm::vec3(-1.0f, -1.0f, 1.0f)),
        VertexP(glm::vec3(-1.0f, -1.0f, -1.0f)),
        VertexP(glm::vec3(1.0f, -1.0f, -1.0f)),
        VertexP(glm::vec3(-1.0f, 1.0f, -1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, -1.)),
    };

    m_skybox.mesh = VulkanMesh("skybox", vertices.size(), sizeof(VertexP), vertices.data());

    std::string skyboxPath     = "../Assets/Skybox/Skybox.png";
    std::string specularPath   = "../Assets/Skybox/specular.png";
    std::string irradiancePath = "../Assets/Skybox/irradiance.png";
    std::string brdfPath       = "../Assets/Skybox/brdf_lut.png";

    m_skybox.skybox     = TextureManager::GetInstance().Load(skyboxPath);
    m_skybox.specular   = TextureManager::GetInstance().Load(specularPath);
    m_skybox.irradiance = TextureManager::GetInstance().Load(irradiancePath);
    m_skybox.brdf       = TextureManager::GetInstance().Load(brdfPath);
};

void VulkanState::CreateRenderObjects() {
    // Lights
    LightManager::GetInstance().Init(m_physicalDevice, m_device);

    // Camera
    Camera::GetInstance().Init();

    CreateSkybox();

    // Descriptor sets
    m_uniformDescriptorSet     = CreateDescriptorSet(m_graphicsPipelines[0]->GetDescriptorSetLayouts()[0]);
    m_uniformViewDescriptorSet = CreateDescriptorSet(m_skyboxPipeline->GetDescriptorSetLayouts()[0]);
    m_skyboxDescriptorSet      = CreateDescriptorSet(m_skyboxPipeline->GetDescriptorSetLayouts()[1]);

    OneTimeUpdateDescriptorSets();

    // UI
    m_ui = std::make_unique<UI>(m_queue, m_imguiDescriptorPool);
}

void VulkanState::ShowUI() {
    m_ui->CameraWindow();
    m_ui->LightsWindow();
    m_uiQueue.Show();
}

void VulkanState::Update() {
    LightManager::GetInstance().Update();
    Camera::GetInstance().Update();
}

void VulkanState::WaitAndResetFence(VkFence fence, uint64_t timeout) {
    DEBUG_VK_ASSERT(vkWaitForFences(m_device, 1, &fence, true, timeout));
    DEBUG_VK_ASSERT(vkResetFences(m_device, 1, &fence));
}

void VulkanState::BeginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferUsageFlags const flag) {
    VkCommandBufferBeginInfo infoBegin{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = flag,
        .pInheritanceInfo = nullptr,
    };
    DEBUG_VK_ASSERT(vkBeginCommandBuffer(cmdBuf, &infoBegin));
}

void VulkanState::EndAndSubmitCommandBuffer(
    VkCommandBuffer            cmdBuf,
    VkPipelineStageFlags const waitStageMask,
    VkFence                    fence,
    VkSemaphore                waitSemaphore,
    VkSemaphore                signalSemaphore
) {
    DEBUG_VK_ASSERT(vkEndCommandBuffer(cmdBuf));

    VkSubmitInfo infoSubmit{
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext              = nullptr,
        .pWaitSemaphores    = &waitSemaphore,
        .pWaitDstStageMask  = &waitStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmdBuf,
        .pSignalSemaphores  = &signalSemaphore,
    };
    if (waitSemaphore != VK_NULL_HANDLE) {
        infoSubmit.waitSemaphoreCount = 1;
    }
    if (signalSemaphore != VK_NULL_HANDLE) {
        infoSubmit.signalSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueueSubmit(m_queue, 1, &infoSubmit, fence));
}

void VulkanState::QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex) {
    VkPresentInfoKHR infoPresent{
        .sType           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext           = nullptr,
        .pWaitSemaphores = &waitSemaphore,
        .swapchainCount  = 1,
        .pSwapchains     = &m_swapchain.swapchain,
        .pImageIndices   = &imageIndex,
        .pResults        = nullptr,
    };
    if (waitSemaphore != VK_NULL_HANDLE) {
        infoPresent.waitSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueuePresentKHR(m_queue, &infoPresent));
}

void VulkanState::OneTimeUpdateDescriptorSets() {
    std::vector<VkDescriptorBufferInfo> infoBuffers{
        {.buffer = Camera::GetInstance().GetBuffer(),       .offset = 0, .range = VK_WHOLE_SIZE},
        {.buffer = LightManager::GetInstance().GetBuffer(), .offset = 0, .range = VK_WHOLE_SIZE}
    };

    VkWriteDescriptorSet writeSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_uniformDescriptorSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(infoBuffers.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = infoBuffers.data(),
        .pTexelBufferView = nullptr,
    };

    VkWriteDescriptorSet writeSetView{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_uniformViewDescriptorSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = infoBuffers.data(),
        .pTexelBufferView = nullptr,
    };

    std::vector<VkDescriptorImageInfo> imageInfo{
        {
         .sampler     = m_skybox.skybox->GetSampler(),
         .imageView   = m_skybox.skybox->GetImageView(),
         .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
         }
    };

    VkWriteDescriptorSet writeSetSkybox{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_skyboxDescriptorSet,
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = static_cast<uint32_t>(imageInfo.size()),
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = imageInfo.data(),
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &writeSet, 0, nullptr);
    vkUpdateDescriptorSets(m_device, 1, &writeSetView, 0, nullptr);
    vkUpdateDescriptorSets(m_device, 1, &writeSetSkybox, 0, nullptr);
}

void VulkanState::DrawImgui(VkImageView view) {
    VkRect2D renderAreas{
        .offset = {0,       0       },
        .extent = {m_width, m_height}
    };
    VkRenderingAttachmentInfo infoColorAttachment = vk_util::GetRenderingAttachmentInfo(
        view,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        nullptr,
        VK_ATTACHMENT_LOAD_OP_LOAD,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_NONE,
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED
    );
    VkRenderingInfo infoRendering = vk_util::GetRenderingInfo(renderAreas, &infoColorAttachment, nullptr);

    vkCmdBeginRendering(m_cmdBuf, &infoRendering);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_cmdBuf);

    vkCmdEndRendering(m_cmdBuf);
}

void VulkanState::Draw() {
    VkRect2D renderAreas{
        .offset = {0,       0       },
        .extent = {m_width, m_height}
    };
    VkClearValue colorClear{
        .color = {0.0f, 0.0f, 0.0f, 1.0f}
    };

    VkRenderingAttachmentInfo infoColorAttachment = vk_util::GetRenderingAttachmentInfo(
        m_msaaColorImage.GetImageView(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        &colorClear,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_AVERAGE_BIT,
        m_drawImage.GetImageView(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    VkClearValue depthClear{
        .depthStencil = {.depth = 1.0f, .stencil = 0}
    };
    VkRenderingAttachmentInfo infoDepthAttachment = vk_util::GetRenderingAttachmentInfo(
        m_depthImage.GetImageView(),
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        &depthClear,
        VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_STORE,
        VK_RESOLVE_MODE_NONE,
        VK_NULL_HANDLE,
        VK_IMAGE_LAYOUT_UNDEFINED
    );

    VkRenderingInfo infoRender = vk_util::GetRenderingInfo(renderAreas, &infoColorAttachment, &infoDepthAttachment);

    vkCmdBeginRendering(m_cmdBuf, &infoRender);

    // Set dynamic viewport and scissor
    VkViewport viewport{
        .x        = 0.f,
        // Flip the view port
        .y        = static_cast<float>(m_height),
        .width    = static_cast<float>(m_width),
        // Flip the view port
        .height   = -static_cast<float>(m_height),
        .minDepth = 0.f,
        .maxDepth = 1.f
    };

    vkCmdSetViewport(m_cmdBuf, 0, 1, &viewport);
    vkCmdSetScissor(m_cmdBuf, 0, 1, &renderAreas);

    DrawSkybox();

    DrawGeometry();

    vkCmdEndRendering(m_cmdBuf);
}

void VulkanState::DrawSkybox() {
    vkCmdBindPipeline(m_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyboxPipeline->GetPipeline());

    vkCmdBindDescriptorSets(m_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyboxPipeline->GetLayout(), 0, 1, &m_uniformViewDescriptorSet, 0, nullptr);
    vkCmdBindDescriptorSets(m_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_skyboxPipeline->GetLayout(), 1, 1, &m_skyboxDescriptorSet, 0, nullptr);

    const VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(m_cmdBuf, 0, 1, &m_skybox.mesh.GetVertexBuffer(), &offset);
    vkCmdDraw(m_cmdBuf, m_skybox.mesh.GetVertexCount(), 1, 0, 0);
}

void VulkanState::DrawGeometry() {
    vkCmdBindPipeline(m_cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipelines[0]->GetPipeline());

    vkCmdBindDescriptorSets(
        m_cmdBuf,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_graphicsPipelines[0]->GetLayout(),
        0,
        1,
        &m_uniformDescriptorSet,
        0,
        nullptr
    );

    for (const auto &instance: m_meshInstances) {
        instance.BindAndDraw(m_cmdBuf);
    }
}

void VulkanState::LoadMeshes() {
    std::vector<std::string> meshPaths{
        "../Assets/Models/Chessboard/Chessboard.obj",
        "../Assets/Models/Castle/Castle.obj",
        "../Assets/Models/BoomBox/BoomBox.obj",
    };

    std::vector<std::string> texturePaths{
        "../Assets/Models/Chessboard/chessboard_base_color.jpg",
        "../Assets/Models/Castle/castle_white_base_color.jpg",
        "../Assets/Models/BoomBox/BoomBox_baseColor.png",
    };
    std::vector<std::string> normalMapPaths{
        "../Assets/Models/Chessboard/chessboard_normal.jpg",
        "../Assets/Models/Castle/Castle_normal.jpg",
        "../Assets/Models/BoomBox/BoomBox_normal.png",
    };
    std::vector<std::string> ormPaths{
        "../Assets/Models/Chessboard/Chessboard_ORM.jpg",
        "../Assets/Models/Castle/Castle_ORM.jpg",
        "../Assets/Models/BoomBox/BoomBox_occlusionRoughnessMetallic.png",
    };
    std::vector<std::string> emissivePaths{
        "",
        "",
        "../Assets/Models/BoomBox/BoomBox_emissive.png",
    };

    std::vector<SamplerConfig> samplerConfigs{
        {},
        {},
        {},
    };
    std::vector<glm::vec3> locations{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.1f, 0.0f),
    };

    for (size_t i = 0; i < meshPaths.size(); i++) {
        size_t index = i;

        DEBUG_ASSERT(m_graphicsPipelines[0] != nullptr);

        const VulkanTexture *baseTexture = TextureManager::GetInstance().Load("albedo");
        const VulkanTexture *normalMap   = TextureManager::GetInstance().Load("normal");
        const VulkanTexture *orm         = TextureManager::GetInstance().Load("orm");
        const VulkanTexture *emissive    = TextureManager::GetInstance().Load("emissive");
        if (texturePaths[i] != "") {
            baseTexture = TextureManager::GetInstance().Load(texturePaths[i]);
        }
        if (normalMapPaths[i] != "") {
            normalMap = TextureManager::GetInstance().Load(normalMapPaths[i]);
        }
        if (ormPaths[i] != "") {
            orm = TextureManager::GetInstance().Load(ormPaths[i]);
        }
        if (emissivePaths[i] != "") {
            emissive = TextureManager::GetInstance().Load(emissivePaths[i]);
        }

        m_meshInstances.emplace_back(
            MeshManager::GetInstance().Load(meshPaths[index]),
            baseTexture,
            normalMap,
            orm,
            emissive,
            m_skybox.brdf,
            m_skybox.specular,
            m_skybox.irradiance,
            m_graphicsPipelines[0],
            locations[index]
        );

        m_uiQueue.instanceUniformScales.push_back(false);
        m_uiQueue.PushFunction([&, index]() {
            bool uniformScale = m_uiQueue.instanceUniformScales[index];
            m_ui->TransformationWindow(m_meshInstances[index], uniformScale, index);
            m_uiQueue.instanceUniformScales[index] = uniformScale;
        });
    }
}
