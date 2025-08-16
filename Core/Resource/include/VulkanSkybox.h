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

    // TODO: may not need these when implementing deferred rendering
    [[nodiscard]] VulkanTexture *GetSpecular() { return m_specular; }

    [[nodiscard]] VulkanTexture *GetIrradiance() { return m_irradiance; }

    [[nodiscard]] VulkanTexture *GetBRDF() { return m_brdf; }

private:
    VulkanMesh    *m_mesh       = nullptr;
    VulkanTexture *m_emissive   = nullptr;
    VulkanTexture *m_specular   = nullptr;
    VulkanTexture *m_irradiance = nullptr;
    VulkanTexture *m_brdf       = nullptr;

    VkDescriptorSet m_cameraSet  = VK_NULL_HANDLE;
    VkDescriptorSet m_textureSet = VK_NULL_HANDLE;

    VkDescriptorSet CreateDescriptorSet(VkDescriptorSetLayout layout);
    void            OneTimeUpdateDescriptorSets();

};