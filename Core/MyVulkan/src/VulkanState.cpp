#include "include/VulkanState.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>

#include <include/VulkanUtil.h>

VulkanState::VulkanState(SDL_Window *window, uint32_t width, uint32_t height)
{
    m_window = window;
    m_width = width;
    m_height = height;

    CreateInstance();
    CreatePhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSurface(window);
    CreateSwapchain(width, height);

    renderFence = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
    renderSemaphore = CreateSemaphore();
    presentSemaphore = CreateSemaphore();
}

VulkanState::~VulkanState()
{
    WaitIdle();

    vkFreeCommandBuffers(device, commandPool, 1, &cmdBuf);
    vkDestroyFence(device, renderFence, nullptr);
    vkDestroySemaphore(device, renderSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);

    vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void VulkanState::CreateInstance()
{
    VkApplicationInfo infoApp
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "VulkanRayTracerApp",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "VulkanRayTracerEngine",
        .apiVersion = VK_API_VERSION_1_0,
    };

    // Enable validation layer
    std::vector<const char *> layers
    {
        "VK_LAYER_KHRONOS_validation",
    };

    uint32_t sdlExtensionCount = 0;
    const char *const *sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    std::vector<const char *> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


    VkInstanceCreateInfo infoInstance
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &infoApp,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    DEBUG_VK_ASSERT(vkCreateInstance(&infoInstance, nullptr, &instance));
}

void VulkanState::CreatePhysicalDevice()
{
    // Query the num of physical devices
    uint32_t physicalDeviceCount = 0;
    DEBUG_VK_ASSERT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
    DEBUG_ASSERT(physicalDeviceCount > 0);
    // Query all physical devices
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    DEBUG_VK_ASSERT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

    // Get the physical device with the max api version(usually the best one)
    uint32_t maxApiVersion = 0;
    for (VkPhysicalDevice p: physicalDevices)
    {
        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(p, &properties);

        if (properties.apiVersion > maxApiVersion)
        {
            physicalDevice = p;
            maxApiVersion = properties.apiVersion;
        }
    }

    VkPhysicalDeviceProperties properties = {0};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    SDL_Log("Selected physical device: %s %d.%d.%d", properties.deviceName,
            VK_API_VERSION_MAJOR(properties.apiVersion), VK_API_VERSION_MINOR(properties.apiVersion),
            VK_API_VERSION_PATCH(properties.apiVersion));
}

void VulkanState::CreateDevice()
{
    float priority = 1.0f;
    // Only use one queue, and use the first one
    VkDeviceQueueCreateInfo infoQueue
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = 0,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    // Enable swapchain extension for presenting on screen
    std::vector<const char *> extensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkDeviceCreateInfo infoDevice
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &infoQueue,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr,
    };
    DEBUG_VK_ASSERT(vkCreateDevice(physicalDevice, &infoDevice, nullptr, &device));

    // Get queue
    vkGetDeviceQueue(device, 0, 0, &queue);
}

void VulkanState::CreateCommandPool()
{
    VkCommandPoolCreateInfo infoCommandPool
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };
    DEBUG_VK_ASSERT(vkCreateCommandPool(device, &infoCommandPool, nullptr, &commandPool));
}

void VulkanState::CreateSurface(SDL_Window *window)
{
    DEBUG_ASSERT(SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface));
}


void VulkanState::CreateSwapchain(uint32_t width, uint32_t height)
{
    VkSwapchainCreateInfoKHR infoSwapchain
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = MIN_SWAPCHAIN_IMG_COUNT,
        .imageFormat = IMG_FORMAT,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = {width, height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = false,
        .oldSwapchain = nullptr,
    };
    DEBUG_VK_ASSERT(vkCreateSwapchainKHR(device, &infoSwapchain, nullptr, &swapchain.swapchain));

    // Get swapchain images
    DEBUG_VK_ASSERT(vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.count, nullptr));
    DEBUG_VK_ASSERT(vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.count, swapchain.images));
}

VkSemaphore VulkanState::CreateSemaphore()
{
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    DEBUG_VK_ASSERT(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));

    return semaphore;
}

VkFence VulkanState::CreateFence(const VkFenceCreateFlags flag)
{
    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flag,
    };
    DEBUG_VK_ASSERT(vkCreateFence(device, &createInfo, nullptr, &fence));

    return fence;
}


void VulkanState::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo infoCmdBuffer
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    DEBUG_VK_ASSERT(vkAllocateCommandBuffers(device, &infoCmdBuffer, &cmdBuf));
}

void VulkanState::WaitIdle()
{
    DEBUG_VK_ASSERT(vkDeviceWaitIdle(device));
}


void VulkanState::Present()
{
    // Reset fence and command buffer
    WaitAndResetFence(renderFence);
    DEBUG_VK_ASSERT(vkResetCommandBuffer(cmdBuf, 0));

    // Acquire next image in the swapchain for presenting
    uint32_t imageIndex = 0;
    DEBUG_VK_ASSERT(
        vkAcquireNextImageKHR(device, swapchain.swapchain, POINT_ONE_SECOND, presentSemaphore, nullptr, &imageIndex));

    VkClearColorValue clearColorValue = {{1.0f, 0.0f, 0.0f, 1.0f}};
    VkImageSubresourceRange clearRange = vk_util::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    BeginCommandBuffer(0);

    // Layout transition so that we can clear image color
    vk_util::CmdImageLayoutTransition(cmdBuf, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT, 0,
                                      VK_ACCESS_TRANSFER_WRITE_BIT);
    // Clear color image
    vkCmdClearColorImage(cmdBuf, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearColorValue, 1,
                         &clearRange);

    // Layout transition for presenting
    vk_util::CmdImageLayoutTransition(cmdBuf, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_GENERAL,
                                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT,
                                      VK_ACCESS_TRANSFER_WRITE_BIT, 0);

    EndAndSubmitCommandBuffer(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, renderFence, presentSemaphore, renderSemaphore);

    QueuePresent(renderSemaphore, imageIndex);
}

void VulkanState::WaitAndResetFence(VkFence fence, uint64_t timeout)
{
    DEBUG_VK_ASSERT(vkWaitForFences(device, 1, &fence, true, timeout));
    DEBUG_VK_ASSERT(vkResetFences(device, 1, &fence));
}

void VulkanState::BeginCommandBuffer(VkCommandBufferUsageFlags const flag)
{
    VkCommandBufferBeginInfo infoBegin{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = flag,
        .pInheritanceInfo = nullptr,
    };
    DEBUG_VK_ASSERT(vkBeginCommandBuffer(cmdBuf, &infoBegin));
}

void VulkanState::EndAndSubmitCommandBuffer(VkPipelineStageFlags const waitStageMask, VkFence fence,
                                            VkSemaphore waitSemaphore, VkSemaphore signalSemaphore)
{
    DEBUG_VK_ASSERT(vkEndCommandBuffer(cmdBuf));

    VkSubmitInfo infoSubmit{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .pWaitSemaphores = &waitSemaphore,
        .pWaitDstStageMask = &waitStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuf,
        .pSignalSemaphores = &signalSemaphore,
    };
    if (waitSemaphore != VK_NULL_HANDLE)
    {
        infoSubmit.waitSemaphoreCount = 1;
    }
    if (signalSemaphore != VK_NULL_HANDLE)
    {
        infoSubmit.signalSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueueSubmit(queue, 1, &infoSubmit, fence));
}

void VulkanState::QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex)
{
    VkPresentInfoKHR infoPresent{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .pWaitSemaphores = &waitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain.swapchain,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };
    if (waitSemaphore != VK_NULL_HANDLE)
    {
        infoPresent.waitSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueuePresentKHR(queue, &infoPresent));
}
