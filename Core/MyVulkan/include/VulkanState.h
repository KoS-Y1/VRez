#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include <glm/glm.hpp>

#include <Debug.h>
#include <Singleton.h>
#include <include/VulkanPrefab.h>

#include "VulkanImage.h"

inline constexpr size_t MIN_SWAPCHAIN_IMG_COUNT = 2;
inline constexpr size_t MAX_SWAPCHAIN_IMG_COUNT = 16;

inline constexpr uint32_t POINT_ONE_SECOND = 100000000;

struct VulkanSwapchain {
    VkSwapchainKHR swapchain                       = VK_NULL_HANDLE;
    VkImage        images[MAX_SWAPCHAIN_IMG_COUNT] = {nullptr};
    VkImageView    views[MAX_SWAPCHAIN_IMG_COUNT]  = {nullptr};
    uint32_t       count                           = 0;
};

struct DeletionQueue {
    std::deque<std::function<void()>> deletors;

    void PushFunction(std::function<void()> &&func) { deletors.push_back(func); }

    void Flush() {
        for (auto it = deletors.rbegin(); it != deletors.rend(); ++it) {
            // Call the function
            (*it)();
        }

        deletors.clear();
    }
};

class VulkanState : public Singleton<VulkanState> {
public:
    void WaitIdle();
    void BeginFrame();
    void EndFrame();
    void CopyToPresentImage(const VulkanImage &image);

    template<class Func>
    void ImmediateSubmit(Func &&func) {
        std::scoped_lock<std::mutex> lock(m_mutex);
        DEBUG_VK_ASSERT(vkResetCommandBuffer(m_immediateCmdBuf, 0));

        BeginCommandBuffer(m_immediateCmdBuf, 0);
        func(m_immediateCmdBuf);
        EndAndSubmitCommandBuffer(m_immediateCmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_immediateFence, VK_NULL_HANDLE, VK_NULL_HANDLE);
        WaitAndResetFence(m_immediateFence);
    }

    [[nodiscard]] const VkPhysicalDevice &GetPhysicalDevice() const { return m_physicalDevice; };

    [[nodiscard]] const VkDevice &GetDevice() const { return m_device; };

    [[nodiscard]] const VkInstance &GetVkInstance() const { return m_instance; };

    [[nodiscard]] const VkCommandBuffer &GetCommandBuffer() const { return m_cmdBuf; };

    [[nodiscard]] const VkDescriptorPool &GetDescriptorPool() const { return m_descriptorPool; }

    [[nodiscard]] const VkQueue &GetQueue() const { return m_queue; };

    [[nodiscard]] const VkImageView &GetPresentImageView() const { return m_swapchain.views[m_presentImageIndex]; }

    [[nodiscard]] const VkImage &GetPresentImage() const { return m_swapchain.images[m_presentImageIndex]; }

    [[nodiscard]] uint32_t GetWidth() const { return m_width; }

    [[nodiscard]] uint32_t GetHeight() const { return m_height; }


protected:
    VulkanState();
    ~VulkanState();

private:
    VkInstance       m_instance       = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice         m_device         = VK_NULL_HANDLE;
    VkQueue          m_queue          = VK_NULL_HANDLE;
    VkCommandPool    m_commandPool    = VK_NULL_HANDLE;
    VkSurfaceKHR     m_surface        = VK_NULL_HANDLE;

    VulkanSwapchain m_swapchain;
    uint32_t        m_presentImageIndex = 0;

    VkFence         m_renderFence      = VK_NULL_HANDLE;
    VkSemaphore     m_renderSemaphore  = VK_NULL_HANDLE;
    VkSemaphore     m_presentSemaphore = VK_NULL_HANDLE;
    VkCommandBuffer m_cmdBuf           = VK_NULL_HANDLE;

    VkCommandBuffer m_immediateCmdBuf = VK_NULL_HANDLE;
    VkFence         m_immediateFence  = VK_NULL_HANDLE;

    VkDescriptorPool m_descriptorPool      = VK_NULL_HANDLE;
    VkDescriptorPool m_imguiDescriptorPool = VK_NULL_HANDLE;

    DeletionQueue m_deletionQueue;

    SDL_Window *m_window = nullptr;
    uint32_t    m_width;
    uint32_t    m_height;

    std::mutex m_mutex;

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

    void WaitAndResetFence(VkFence fence, uint64_t timeout = POINT_ONE_SECOND);

    static void BeginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferUsageFlags flag);

    void EndAndSubmitCommandBuffer(
        VkCommandBuffer      cmdBuf,
        VkPipelineStageFlags waitStageMask,
        VkFence              fence,
        VkSemaphore          waitSemaphore,
        VkSemaphore          signalSemaphore
    );

    void QueuePresent(VkSemaphore waitSemaphore);

    void AcquireNextImage();
};
