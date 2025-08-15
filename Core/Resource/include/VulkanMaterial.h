#pragma once

#include <vulkan/vulkan.h>

class VulkanTexture;

class VulkanMaterial {
public:
    VulkanMaterial(
        const VulkanTexture        *albedo,
        const VulkanTexture        *normal,
        const VulkanTexture        *orm,
        const VulkanTexture        *emissive,
        const VkDescriptorSetLayout layout
    )
        : m_albedo(albedo)
        , m_normal(normal)
        , m_orm(orm)
        , m_emissive(emissive) {
        CreateDescriptorSets(layout);
    }

    VulkanMaterial() = delete;

    ~VulkanMaterial() { Destroy(); }

    VulkanMaterial(const VulkanMaterial &)            = delete;
    VulkanMaterial(VulkanMaterial &&)                 = delete;
    VulkanMaterial &operator=(const VulkanMaterial &) = delete;
    VulkanMaterial &operator=(VulkanMaterial &&)      = delete;

    void Destroy();

    void Bind(VkCommandBuffer cmdBuf, VkPipelineLayout layout, uint32_t firstSet);

private:
    const VulkanTexture *m_albedo   = nullptr;
    const VulkanTexture *m_normal   = nullptr;
    const VulkanTexture *m_orm      = nullptr;
    const VulkanTexture *m_emissive = nullptr;

    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

    void CreateDescriptorSets(const VkDescriptorSetLayout layout);
};