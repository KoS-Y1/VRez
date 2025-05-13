#pragma once

#include <vector>
#include <deque>
#include <functional>

#include <Assert.h>
#include <include/VulkanImage.h>


#define MIN_SWAPCHAIN_IMG_COUNT 2
#define MAX_SWAPCHAIN_IMG_COUNT 16

#define POINT_ONE_SECOND 100000000u

struct VulkanSwapchain
{
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkImage images[MAX_SWAPCHAIN_IMG_COUNT] = {0};
    VkImageView views[MAX_SWAPCHAIN_IMG_COUNT] = {0};
    uint32_t count = 0;
};

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void pushFunction(std::function<void()> &&func)
    {
        deletors.push_back(func);
    }

    void flush()
    {
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
        {
            // Call the function
            (*it)();
        }

        deletors.clear();
    }
};


class VulkanState
{
public:
    VulkanState(SDL_Window *window, uint32_t width, uint32_t height);

    ~VulkanState();

    // Disallow copy and move
    VulkanState (const VulkanState &) = delete;
    VulkanState (VulkanState &&) = delete;
    VulkanState& operator=(VulkanState const &) = delete;
    VulkanState& operator=(VulkanState &&) = delete;

    [[nodiscard]] VkDevice const &GetDevice() const { return device; };

    void Present();

private:
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VulkanSwapchain swapchain;

    VkFence renderFence = VK_NULL_HANDLE;
    VkSemaphore renderSemaphore = VK_NULL_HANDLE;
    VkSemaphore presentSemaphore = VK_NULL_HANDLE;
    VkCommandBuffer cmdBuf = VK_NULL_HANDLE;

    VulkanImage drawImage;

    SDL_Window *m_window = nullptr;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    DeletionQueue deletionQueue;

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
