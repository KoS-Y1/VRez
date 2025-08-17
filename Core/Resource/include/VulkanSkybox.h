#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanTexture;
class VulkanMesh;
class VulkanGraphicsPipeline;

class VulkanSkybox {
public:
    VulkanSkybox() = default;

    VulkanSkybox(const std::string &jsonFile, std::vector<VkDescriptorSetLayout> layouts);

    ~VulkanSkybox() { Destroy(); }

    VulkanSkybox(const VulkanSkybox &)            = delete;
    VulkanSkybox &operator=(const VulkanSkybox &) = delete;

    VulkanSkybox(VulkanSkybox &&other) { Swap(other); }

    VulkanSkybox &operator=(VulkanSkybox &&other) {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void Destroy();

    void Swap(VulkanSkybox &other) noexcept;

    void BindAndDraw(VkCommandBuffer cmdBuf, const VulkanGraphicsPipeline *skybox);

    [[nodiscard]] const VkDescriptorSet &GetIBLSet(std::string pipeline) { return m_iblSet; }

    // TODO: may not need these when implementing deferred rendering
    [[nodiscard]] const VulkanTexture *GetSpecular() { return m_specular; }

    [[nodiscard]] const VulkanTexture *GetIrradiance() { return m_irradiance; }

    [[nodiscard]] const VulkanTexture *GetBRDF() { return m_brdf; }

private:
    const VulkanMesh    *m_mesh       = nullptr;
    const VulkanTexture *m_emissive   = nullptr;
    const VulkanTexture *m_specular   = nullptr;
    const VulkanTexture *m_irradiance = nullptr;
    const VulkanTexture *m_brdf       = nullptr;

    VkDescriptorSet m_cameraSet  = VK_NULL_HANDLE;
    VkDescriptorSet m_textureSet = VK_NULL_HANDLE;
    VkDescriptorSet m_iblSet     = VK_NULL_HANDLE;

    VkDescriptorSet CreateDescriptorSet(VkDescriptorSetLayout layout);
    void            OneTimeUpdateDescriptorSets();
};