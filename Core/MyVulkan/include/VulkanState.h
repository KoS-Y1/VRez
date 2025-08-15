#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <mutex>

#include <glm/glm.hpp>

#include <Debug.h>
#include <Singleton.h>
#include <include/UI.h>

#include "VulkanComputePipeline.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanImage.h"
#include "VulkanMesh.h"
#include "VulkanPrefab.h"
#include "VulkanTexture.h"

#define MIN_SWAPCHAIN_IMG_COUNT 2
#define MAX_SWAPCHAIN_IMG_COUNT 16

#define POINT_ONE_SECOND 100000000u


struct Skybox {
    const VulkanTexture *skybox;
    const VulkanTexture *specular;
    const VulkanTexture *irradiance;
    const VulkanTexture *brdf;
    VulkanMesh           mesh;

    void Destroy() {
        skybox = nullptr;
        mesh.Destroy();
    }
};

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

struct UIQueue {
    std::deque<std::function<void()>> ui;

    void PushFunction(std::function<void()> &&func) { ui.push_back(func); }

    void Show() {
        for (auto it = ui.begin(); it != ui.end(); ++it) {
            (*it)();
        }
    }

    std::vector<bool> instanceUniformScales;
};

class VulkanState : public Singleton<VulkanState>{
public:
    void Init();

    void Destroy();

    [[nodiscard]] VkPhysicalDevice const &GetPhysicalDevice() const { return m_physicalDevice; };

    [[nodiscard]] VkDevice const &GetDevice() const { return m_device; };

    [[nodiscard]] VkInstance const &GetVkInstance() const { return m_instance; };

    void WaitIdle();

    void Present();

    void ShowUI();

    void Update();

    template<class Func>
    void ImmediateSubmit(Func &&func) {
        std::scoped_lock<std::mutex> lock(m_mutex);
        DEBUG_VK_ASSERT(vkResetCommandBuffer(m_immediateCmdBuf, 0));

        BeginCommandBuffer(m_immediateCmdBuf, 0);
        func(m_immediateCmdBuf);
        EndAndSubmitCommandBuffer(m_immediateCmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_immediateFence, VK_NULL_HANDLE, VK_NULL_HANDLE);
        WaitAndResetFence(m_immediateFence);
    }

protected:
    VulkanState() = default;

    ~VulkanState() = default;

private:
    VkInstance       m_instance       = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice         m_device         = VK_NULL_HANDLE;
    VkQueue          m_queue          = VK_NULL_HANDLE;
    VkCommandPool    m_commandPool    = VK_NULL_HANDLE;
    VkSurfaceKHR     m_surface        = VK_NULL_HANDLE;
    VulkanSwapchain  m_swapchain;

    VkFence         m_renderFence      = VK_NULL_HANDLE;
    VkSemaphore     m_renderSemaphore  = VK_NULL_HANDLE;
    VkSemaphore     m_presentSemaphore = VK_NULL_HANDLE;
    VkCommandBuffer m_cmdBuf           = VK_NULL_HANDLE;

    VkCommandBuffer m_immediateCmdBuf = VK_NULL_HANDLE;
    VkFence         m_immediateFence  = VK_NULL_HANDLE;

    VkDescriptorPool m_descriptorPool      = VK_NULL_HANDLE;
    VkDescriptorPool m_imguiDescriptorPool = VK_NULL_HANDLE;

    VkDescriptorSet m_uniformDescriptorSet     = VK_NULL_HANDLE;
    VkDescriptorSet m_uniformViewDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSet m_skyboxDescriptorSet      = VK_NULL_HANDLE;

    VulkanImage m_drawImage;
    VulkanImage m_depthImage;
    VulkanImage m_msaaColorImage;

    std::vector<std::shared_ptr<VulkanComputePipeline>>  m_computePipelines;
    std::vector<std::shared_ptr<VulkanGraphicsPipeline>> m_graphicsPipelines;
    std::unique_ptr<VulkanGraphicsPipeline>              m_skyboxPipeline;

    std::vector<VulkanPrefab> m_meshInstances;

    VulkanTexture m_albedoTexture;
    VulkanTexture m_normalMap;
    VulkanTexture m_ormTexture;
    VulkanTexture m_emissiveTexture;

    VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_1_BIT;

    Skybox m_skybox;

    DeletionQueue m_deletionQueue;
    UIQueue       m_uiQueue;

    SDL_Window *m_window = nullptr;
    uint32_t    m_width;
    uint32_t    m_height;

    std::unique_ptr<UI> m_ui;

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

    VkDescriptorSet CreateDescriptorSet(VkDescriptorSetLayout layout);

    void CreatePipelines();

    void CreateTextures();

    void CreateSkybox();

    void CreateRenderObjects();

    void WaitAndResetFence(VkFence fence, uint64_t timeout = POINT_ONE_SECOND);

    static void BeginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferUsageFlags flag);

    void EndAndSubmitCommandBuffer(
        VkCommandBuffer      cmdBuf,
        VkPipelineStageFlags waitStageMask,
        VkFence              fence,
        VkSemaphore          waitSemaphore,
        VkSemaphore          signalSemaphore
    );

    void QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex);

    void Draw();

    void DrawSkybox();

    void DrawGeometry();

    void OneTimeUpdateDescriptorSets();

    void DrawImgui(VkImageView view);

    // Meshes
    void LoadMeshes();
};
