#include "include/VulkanObject.h"

#include "include/VulkanMesh.h"

#include <include/DescriptorSets.h>
#include <include/VulkanMaterial.h>

VulkanObject::VulkanObject(const VulkanMesh *mesh, const VulkanMaterial *material) {
    m_mesh = mesh;
    m_material = material;
}

void VulkanObject::Swap(VulkanObject &other) noexcept {
    std::swap(m_mesh, other.m_mesh);
    std::swap(m_material, other.m_material);
}

void VulkanObject::Destroy() {
    m_mesh     = nullptr;
    m_material = nullptr;
}

void VulkanObject::BindAndDraw(VkCommandBuffer cmdBuf, VkPipelineLayout layout) const {
    m_material->Bind(cmdBuf, layout, TEXTURE_SET);
    m_mesh->BindAndDraw(cmdBuf);
}
