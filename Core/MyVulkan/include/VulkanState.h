#pragma once

#include <vector>
#include <deque>
#include <functional>
#include <memory>

#include <Debug.h>

#include "VulkanImage.h"
#include "VulkanPipeline.h"


#define MIN_SWAPCHAIN_IMG_COUNT 2
#define MAX_SWAPCHAIN_IMG_COUNT 16


#define POINT_ONE_SECOND 100000000u


struct VulkanSwapchain
{
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkImage images[MAX_SWAPCHAIN_IMG_COUNT] = {nullptr};
    VkImageView views[MAX_SWAPCHAIN_IMG_COUNT] = {nullptr};
    uint32_t count = 0;
};

struct DeletionQueue
{
    std::deque<std::function<void()> > deletors;

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

class MeshLoader;
class VulkanMesh;

class VulkanState
{
public:
    VulkanState(SDL_Window *window, uint32_t width, uint32_t height);

    ~VulkanState();

    // Disallow copy and move
    VulkanState(const VulkanState &) = delete;

    // VulkanState(VulkanState &&) = delete;

    VulkanState &operator=(VulkanState const &) = delete;

    VulkanState &operator=(VulkanState &&) = delete;

    [[nodiscard]] VkInstance const &GetInstance() const { return m_instance; };
    [[nodiscard]] VkPhysicalDevice const &GetPhysicalDevice() const { return m_physicalDevice; };
    [[nodiscard]] VkDevice const &GetDevice() const { return m_device; };
    [[nodiscard]] VkQueue const &GetQueue() const { return m_queue; };
    [[nodiscard]] VkDescriptorPool const &GetImGuiDescriptorPool() { return m_imguiDescriptorPool; };

    void Present();

    void WaitIdle();

    template<class Func>
    void ImmediateSubmit(Func &&func)
    {
        // Wait and reset command buffer and fence
        WaitAndResetFence(m_immediateFence);
        DEBUG_VK_ASSERT(vkResetCommandBuffer(m_cmdBuf, 0));

        BeginCommandBuffer(m_immediateCmdBuf, 0);
        func(m_cmdBuf);
        EndAndSubmitCommandBuffer(m_immediateCmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_immediateFence,
                                  VK_NULL_HANDLE, VK_NULL_HANDLE);
    }

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_queue = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VulkanSwapchain m_swapchain;

    VkFence m_renderFence = VK_NULL_HANDLE;
    VkSemaphore m_renderSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_presentSemaphore = VK_NULL_HANDLE;
    VkCommandBuffer m_cmdBuf = VK_NULL_HANDLE;

    VkCommandBuffer m_immediateCmdBuf = VK_NULL_HANDLE;
    VkFence m_immediateFence = VK_NULL_HANDLE;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorPool m_imguiDescriptorPool = VK_NULL_HANDLE;

    VulkanImage m_drawImage;
    std::vector<std::unique_ptr<VulkanPipeline> > m_pipelines;

    std::unique_ptr<MeshLoader> m_meshLoader;

    SDL_Window *m_window = nullptr;
    uint32_t m_width;
    uint32_t m_height;

    DeletionQueue deletionQueue;

    std::vector<VkDescriptorSet> descriptorSets;

    void CreateInstance();

    void CreatePhysicalDevice();

    void CreateDevice();

    void CreateCommandPool();

    void CreateSurface(SDL_Window *window);

    void CreateSwapchain(uint32_t width, uint32_t height);

    void CreateCommandBuffer();

    VkSemaphore CreateSemaphore();

    VkFence CreateFence(VkFenceCreateFlags flag);

    void CreateDescriptorPool();

    void CreateDescriptorSet(VkDescriptorSetLayout layout);

    void CreatePipelines();

    void WaitAndResetFence(VkFence fence, uint64_t timeout = POINT_ONE_SECOND);

    void BeginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferUsageFlags  flag);

    void EndAndSubmitCommandBuffer(VkCommandBuffer cmdBuf, VkPipelineStageFlags  waitStageMask, VkFence fence,
                                   VkSemaphore waitSemaphore, VkSemaphore signalSemaphore);

    void QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex);

    void DrawBackground();

    void DrawGeometry();

    void UpdateDescriptorSets();

    void DrawImgui(VkImageView view);

    void BindAndDrawMesh(const VulkanMesh *mesh);
};
