#pragma once

#include <vector>
#include <deque>
#include <functional>
#include <memory>

#include <glm/glm.hpp>

#include <Debug.h>
#include <string>

#include "VulkanImage.h"
#include "VulkanBuffer.h"


#define MIN_SWAPCHAIN_IMG_COUNT 2
#define MAX_SWAPCHAIN_IMG_COUNT 16

#define POINT_ONE_SECOND 100000000u

class VulkanComputePipeline;
class VulkanGraphicsPipeline;
class MeshLoader;
class VulkanMesh;
class MeshInstance;
class UI;

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

    void PushFunction(std::function<void()> &&func)
    {
        deletors.push_back(func);
    }

    void Flush()
    {
        for (auto it = deletors.rbegin(); it != deletors.rend(); ++it)
        {
            // Call the function
            (*it)();
        }

        deletors.clear();
    }
};

struct UIQueue
{
    std::deque<std::function<void()> > ui;

    void PushFunction(std::function<void()> &&func)
    {
        ui.push_back(func);
    }

    void Show()
    {
        for (auto it = ui.begin(); it != ui.end(); ++it)
        {
            (*it)();
        }
    }

    std::vector<bool> instanceUniformScales;
};

class VulkanState
{
public:
    VulkanState(SDL_Window *window, uint32_t width, uint32_t height);

    ~VulkanState();

    // Disallow copy and move
    VulkanState(const VulkanState &) = delete;

    VulkanState(VulkanState &&) = delete;

    VulkanState &operator=(VulkanState const &) = delete;

    VulkanState &operator=(VulkanState &&) = delete;

    [[nodiscard]] VkPhysicalDevice const &GetPhysicalDevice() const { return m_physicalDevice; };
    [[nodiscard]] VkDevice const &GetDevice() const { return m_device; };

    void WaitIdle();

    void Present();

    void ShowUI();

    void UpdatView(const glm::mat4 &view, const glm::mat4 &projection);

    template<class Func>
    void ImmediateSubmit(Func &&func)
    {
        DEBUG_VK_ASSERT(vkResetCommandBuffer(m_cmdBuf, 0));

        BeginCommandBuffer(m_immediateCmdBuf, 0);
        func(m_immediateCmdBuf);
        EndAndSubmitCommandBuffer(m_immediateCmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_immediateFence,
                                  VK_NULL_HANDLE, VK_NULL_HANDLE);
        WaitAndResetFence(m_immediateFence);
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

    VulkanBuffer m_viewBuffer;
    VkDescriptorSet m_computeDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSet m_uniformViewDescriptorSet = VK_NULL_HANDLE;

    VulkanImage m_drawImage;

    std::vector<std::shared_ptr<VulkanComputePipeline>> m_computePipelines;
    std::vector<std::shared_ptr<VulkanGraphicsPipeline>> m_graphicsPipelines;

    std::unique_ptr<MeshLoader> m_meshLoader;
    std::vector<MeshInstance> m_meshInstances;

    DeletionQueue m_deletionQueue;
    UIQueue m_uiQueue;

    SDL_Window *m_window = nullptr;
    uint32_t m_width;
    uint32_t m_height;

    std::unique_ptr<UI> m_ui;

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

    VkDescriptorSet CreateDescriptorSet(VkDescriptorSetLayout layout);

    void CreatePipelines();

    void WaitAndResetFence(VkFence fence, uint64_t timeout = POINT_ONE_SECOND);

    static void BeginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferUsageFlags flag);

    void EndAndSubmitCommandBuffer(VkCommandBuffer cmdBuf, VkPipelineStageFlags waitStageMask, VkFence fence,
                                   VkSemaphore waitSemaphore, VkSemaphore signalSemaphore);

    void QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex);

    void DrawBackground();

    void DrawGeometry();

    void OneTimeUpdateDescriptorSets();

    void DrawImgui(VkImageView view);


    // Meshes
    void LoadMeshes();

    void BindAndDrawMesh(const MeshInstance &instance);
};
