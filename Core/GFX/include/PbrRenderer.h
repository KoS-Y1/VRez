#pragma once


#include <vector>

#include <include/ForwardPass.h>
#include <include/GBufferPass.h>
#include <include/LightingPass.h>
#include <include/ShadowPass.h>
#include <include/SkyboxPass.h>
#include <include/UIRenderer.h>
#include <include/VulkanImage.h>
#include <include/VulkanPrefab.h>

struct DrawContent {
    std::vector<VulkanPrefab> deferredPrefabs;
    std::vector<VulkanPrefab> frontPrefabs;
    VulkanMesh               *screen;
};

class PbrRenderer {
public:
    PbrRenderer() = delete;

    explicit PbrRenderer(UIRenderer &uiRenderer);

    ~PbrRenderer();

    PbrRenderer(const PbrRenderer &)            = delete;
    PbrRenderer(PbrRenderer &&)                 = delete;
    PbrRenderer &operator=(const PbrRenderer &) = delete;
    PbrRenderer &operator=(PbrRenderer &&)      = delete;

    void Render();

    [[nodiscard]] const VkExtent3D &GetDrawImageExtent() const { return m_drawImage.GetExtent(); }

    [[nodiscard]] const VkImage &GetDrawImage() const { return m_drawImage.GetImage(); }

private:
    VulkanImage m_depthImage;
    VulkanImage m_drawImage;

    RenderingConfig m_config;

    ShadowPass   m_shadowPass;
    GBufferPass  m_gBufferPass;
    LightingPass m_lightingPass;
    ForwardPass  m_forwardPass;
    SkyboxPass   m_skybox;

    DrawContent m_drawContent;

    VulkanTexture *m_brdf       = nullptr;
    VulkanTexture *m_irradiance = nullptr;
    VulkanTexture *m_specular   = nullptr;

    VulkanBuffer m_cameraBuffer;
    VulkanBuffer m_lightBuffer;

    VkDescriptorSet m_uniformSet       = VK_NULL_HANDLE;
    VkDescriptorSet m_cameraSet        = VK_NULL_HANDLE;
    VkDescriptorSet m_iblSet           = VK_NULL_HANDLE;
    VkDescriptorSet m_uniformShadowSet = VK_NULL_HANDLE;
    VkDescriptorSet m_uniformForwardSet   = VK_NULL_HANDLE;

    void CreateImages();
    void CreateDrawContent();
    void CreateBuffers();
    void CreateDescriptorSets();
    void CreateRenderConfig();
    void OneTimeUpdateDescriptorSets();
};