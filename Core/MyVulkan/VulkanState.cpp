#include "VulkanState.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>

VulkanState::VulkanState()
{
    CreateInstance();
    CreatePhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateCommandBuffer();
}

VulkanState::~VulkanState()
{
    WaitIdle();

    vkFreeCommandBuffers(device,commandPool, 1, &cmdBuf);
    vkDestroyCommandPool(device,commandPool,nullptr);
    vkDestroyDevice(device,nullptr);
    vkDestroyInstance(instance,nullptr);
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
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
}

void VulkanState::CreateCommandPool()
{
    VkCommandPoolCreateInfo infoCommandPool
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = 0,
        };
    DEBUG_VK_ASSERT(vkCreateCommandPool(device, &infoCommandPool, nullptr, &commandPool));
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

