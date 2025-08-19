#pragma once

#include "VulkanMesh.h"


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

    void BindAndDraw(VkPipelineLayout layout) const;

    [[nodiscard]] const std::string &GetName() const { return m_mesh->GetName(); }

private:
    const VulkanMesh     *m_mesh     = nullptr;
    const VulkanMaterial *m_material = nullptr;
};