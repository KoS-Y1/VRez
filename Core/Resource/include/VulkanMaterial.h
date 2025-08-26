#pragma once

#include <vulkan/vulkan.h>

#include <include/VulkanGraphicsPipeline.h>

class VulkanTexture;

class VulkanMaterial {
public:
    VulkanMaterial(
        const VulkanTexture          *albedo,
        const VulkanTexture          *normal,
        const VulkanTexture          *orm,
        const VulkanTexture          *emissive,
        const VulkanGraphicsPipeline *pipeline
    );

    VulkanMaterial() = delete;

    ~VulkanMaterial() { Destroy(); }

    VulkanMaterial(const VulkanMaterial &)            = delete;
    VulkanMaterial &operator=(const VulkanMaterial &) = delete;

    VulkanMaterial(VulkanMaterial &&other) { Swap(other); }

    VulkanMaterial &operator=(VulkanMaterial &&other) {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void Swap(VulkanMaterial &other) noexcept;

    void Destroy();

    void Bind(VkPipelineLayout layout, uint32_t firstSet) const;

private:
    const VulkanTexture *m_albedo   = nullptr;
    const VulkanTexture *m_normal   = nullptr;
    const VulkanTexture *m_orm      = nullptr;
    const VulkanTexture *m_emissive = nullptr;

    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

    void OneTimeUpdateDescriptorSets();
};