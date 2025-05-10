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

    [[nodiscard]] VkDevice const &GetDevice() const { return device; };

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

    SDL_Window *m_window = nullptr;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    void CreateInstance();

    void CreatePhysicalDevice();

    void CreateDevice();

    void CreateCommandPool();

    void CreateSurface(SDL_Window *window);

    void CreateSwapchain(uint32_t width, uint32_t height);

    void CreateCommandBuffer();

    VkSemaphore CreateSemaphore() ;

    VkFence CreateFence(VkFenceCreateFlags const flag) ;

    void WaitIdle() ;

    void WaitAndResetFence(VkFence fence, uint64_t timeout = POINT_ONE_SECOND) ;

    void BeginCommandBuffer(VkCommandBufferUsageFlags const flag) ;

    void EndAndSubmitCommandBuffer(VkPipelineStageFlags const waitStageMask, VkFence fence,
                                   VkSemaphore waitSemaphore, VkSemaphore signalSemaphore);

    void QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex) ;
};
