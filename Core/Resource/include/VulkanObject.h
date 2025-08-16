#pragma once

#include <vulkan/vulkan.h>

#include <include/TextureManager.h>

class VulkanMaterial;
class VulkanMesh;
class VulkanTexture;
class VulkanPipeline;

class VulkanObject {
public:
    VulkanObject() = default;

    ~VulkanObject() { Destroy(); }

    // TODO: ibl stuff should be in deferred rendering
    VulkanObject(VulkanMesh *mesh, VulkanMaterial *material);


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

    void BindAndDraw(VkCommandBuffer cmdBuf);

private:
    const VulkanMesh     *m_mesh     = nullptr;
    const VulkanMaterial *m_material = nullptr;

    // TODO: ibl stuff should be in deferred rendering
    const VulkanTexture *m_specular   = nullptr;
    const VulkanTexture *m_irradiance = nullptr;
    const VulkanTexture *m_brdf       = nullptr;
    VkDescriptorSet      m_iblSet     = VK_NULL_HANDLE;
    void                 CreateDescriptorSet();
};