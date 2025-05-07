#pragma once

#include <vulkan/vulkan.h>
#include <Singleton.h>
#include <Assert.h>

class VulkanState
{
public:
    VulkanState();
    ~VulkanState();

    // Disallow copy and move constructor and operator
    VulkanState(VulkanState &&) = delete;
    VulkanState(VulkanState const &) = delete;
    VulkanState &operator=(VulkanState &&) = delete;
    VulkanState &operator=(VulkanState const &) = delete;

private:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer cmdBuf;

    void CreateInstance();
    void CreatePhysicalDevice();
    void CreateDevice();
    void CreateCommandPool();
    void CreateCommandBuffer();

    void WaitIdle();
};