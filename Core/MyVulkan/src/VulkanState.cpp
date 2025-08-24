#include "include/VulkanState.h"

#include "include/ThreadPool.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>
#include <imgui_impl_vulkan.h>
#include <include/VulkanUtil.h>

#include <include/Descriptor.h>
#include <include/Window.h>

VulkanState::VulkanState() {
    m_window = Window::GetInstance().GetSDLWindow();
    m_width  = Window::GetInstance().GetWidth();
    m_height = Window::GetInstance().GetHeight();

    CreateInstance();
    CreatePhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSurface(Window::GetInstance().GetSDLWindow());
    CreateSwapchain(Window::GetInstance().GetWidth(), Window::GetInstance().GetHeight());

    m_renderFence      = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
    m_immediateFence   = CreateFence(0);
    m_renderSemaphore  = CreateSemaphore();
    m_presentSemaphore = CreateSemaphore();

    CreateDescriptorPool();
}

VulkanState::~VulkanState() {
    if (m_device == VK_NULL_HANDLE) {
        return;
    }

    WaitIdle();

    for (size_t i = 0; i < m_swapchain.count; i++) {
        vkDestroyImageView(m_device, m_swapchain.views[i], nullptr);
    }

    TextureManager::GetInstance().Destroy();

    m_deletionQueue.Flush();

    m_device = VK_NULL_HANDLE;
}

void VulkanState::BeginFrame() {
    // Reset fence and command buffer
    WaitAndResetFence(m_renderFence);
    DEBUG_VK_ASSERT(vkResetCommandBuffer(m_cmdBuf, 0));

    AcquireNextImage();

    BeginCommandBuffer(m_cmdBuf, 0);
}

void VulkanState::EndFrame() {
    // Layout transition for presenting
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_swapchain.images[m_presentImageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        0
    );

    EndAndSubmitCommandBuffer(m_cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_renderFence, m_presentSemaphore, m_renderSemaphore);

    QueuePresent(m_renderSemaphore);
}

void VulkanState::CopyToPresentImage(const VulkanImage &image) {
    vk_util::CmdImageLayoutTransition(
        m_cmdBuf,
        m_swapchain.images[m_presentImageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );

    // Copy draw image to the current swapchain image
    vk_util::CopyImageToImage(
        m_cmdBuf,
        image.GetImage(),
        m_swapchain.images[m_presentImageIndex],
        image.GetExtent(),
        {m_width, m_height, 1},
        VK_IMAGE_ASPECT_COLOR_BIT
    );
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
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nullptr,
        .flags                 = 0,
        .surface               = m_surface,
        .minImageCount         = MIN_SWAPCHAIN_IMG_COUNT,
        .imageFormat           = VK_FORMAT_R16G16B16A16_SFLOAT,
        .imageColorSpace       = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent           = {width, height},
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
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
            .format   = VK_FORMAT_R16G16B16A16_SFLOAT,
            .components =
                {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = vk_util::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT)
        };

        for (size_t i = 0; i < m_swapchain.count; i++) {
            infoView.image = m_swapchain.images[i];
            DEBUG_VK_ASSERT(vkCreateImageView(m_device, &infoView, nullptr, &m_swapchain.views[i]));
        }
    }

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
        .maxSets       = descriptor::MAX_SET_COUNT,
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

void VulkanState::WaitIdle() {
    DEBUG_VK_ASSERT(vkDeviceWaitIdle(m_device));
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

void VulkanState::QueuePresent(VkSemaphore waitSemaphore) {
    VkPresentInfoKHR infoPresent{
        .sType           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext           = nullptr,
        .pWaitSemaphores = &waitSemaphore,
        .swapchainCount  = 1,
        .pSwapchains     = &m_swapchain.swapchain,
        .pImageIndices   = &m_presentImageIndex,
        .pResults        = nullptr,
    };
    if (waitSemaphore != VK_NULL_HANDLE) {
        infoPresent.waitSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueuePresentKHR(m_queue, &infoPresent));
}

void VulkanState::AcquireNextImage() {
    // Acquire next image in the swapchain for presenting
    DEBUG_VK_ASSERT(vkAcquireNextImageKHR(m_device, m_swapchain.swapchain, POINT_ONE_SECOND, m_presentSemaphore, nullptr, &m_presentImageIndex));
}
