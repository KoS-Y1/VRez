#include "include/VulkanObject.h"

#include <include/Descriptor.h>
#include <include/VulkanMaterial.h>
#include <include/VulkanMesh.h>
#include <include/VulkanState.h>

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

void VulkanObject::BindAndDraw(VkPipelineLayout layout) const {
    m_material->Bind(layout, descriptor::TEXTURE_SET);
    m_mesh->BindAndDraw();
}
