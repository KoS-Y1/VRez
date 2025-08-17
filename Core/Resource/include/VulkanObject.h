#pragma once

#include <vulkan/vulkan.h>

#include <include/TextureManager.h>

class VulkanMaterial;
class VulkanMesh;
class VulkanTexture;
class VulkanGraphicsPipeline;

class VulkanObject {
public:
    VulkanObject() = default;

    ~VulkanObject() { Destroy(); }

    VulkanObject(const VulkanMesh *mesh, const VulkanMaterial *material);

    VulkanObject(const VulkanObject &)            = delete;
    VulkanObject &operator=(const VulkanObject &) = delete;

    VulkanObject(VulkanObject &&other) { Swap(other); }

    VulkanObject &operator=(VulkanObject &&other) {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void Swap(VulkanObject &other) noexcept;
    void Destroy();

    void BindAndDraw(VkCommandBuffer cmdBuf, VkPipelineLayout layout) const;

private:
    const VulkanMesh     *m_mesh     = nullptr;
    const VulkanMaterial *m_material = nullptr;

};