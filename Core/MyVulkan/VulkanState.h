#pragma once

#include <vector>

#include <Singleton.h>
#include <Assert.h>

#define MIN_SWAPCHAIN_IMG_COUNT 2
#define MAX_SWAPCHAIN_IMG_COUNT 16
#define IMG_FORMAT VK_FORMAT_B8G8R8A8_SRGB

#define POINT_ONE_SECOND 100000000u

struct VulkanSwapchain
{
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkImage images[MAX_SWAPCHAIN_IMG_COUNT] = {0};
    uint32_t count = 0;
};

class VulkanState : public Singleton<VulkanState>
{
public:
    VulkanState(SDL_Window *window, uint32_t width, uint32_t height);

    ~VulkanState();

    void Present();

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VulkanSwapchain swapchain = {0};

    VkFence renderFence = VK_NULL_HANDLE;
    VkSemaphore renderSemaphore = VK_NULL_HANDLE;
    VkSemaphore presentSemaphore = VK_NULL_HANDLE;
    VkCommandBuffer cmdBuf = VK_NULL_HANDLE;

    SDL_Window *window = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;

    void CreateInstance();

    void CreatePhysicalDevice();

    void CreateDevice();

    void CreateCommandPool();

    void CreateSurface(SDL_Window *window);

    void CreateSwapchain(uint32_t width, uint32_t height);

    void CreateCommandBuffer();

    VkSemaphore CreateSemaphore() const;

    VkFence CreateFence(VkFenceCreateFlags const flag) const;

    void WaitIdle() const;

    void WaitAndResetFence(VkFence fence, uint64_t timeout = POINT_ONE_SECOND) const;

    void BeginCommandBuffer(VkCommandBufferUsageFlags const flag) const;

    void EndAndSubmitCommandBuffer(VkPipelineStageFlags const waitStageMask, VkFence const fence,
                                   VkSemaphore const waitSemaphore, VkSemaphore const signalSemaphore);

    void QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex) const;
};
