#include "include/VulkanObject.h"

#include <any>

VulkanObject::VulkanObject(VulkanMesh *mesh, VulkanMaterial *material) {
    m_mesh = mesh;
    m_material = material;

    // TODO: ibl stuff should be in deferred rendering
    m_specular   = TextureManager::GetInstance().Load("../Assets/Skybox/specular.png");
    m_irradiance = TextureManager::GetInstance().Load("../Assets/Skybox/irradiance.png");
    m_brdf       = TextureManager::GetInstance().Load("../Assets/Skybox/brdf_lut.png");
    CreateDescriptorSet();
}

void VulkanObject::Swap(VulkanObject &other) noexcept {
    std::swap(m_mesh, other.m_mesh);
    std::swap(m_material, other.m_material);
}

void VulkanObject::Destroy() {
    m_mesh     = nullptr;
    m_material = nullptr;
}

void VulkanObject::BindAndDraw(VkCommandBuffer cmdBuf) {

}
