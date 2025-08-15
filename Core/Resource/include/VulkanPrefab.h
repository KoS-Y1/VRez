#pragma once

#include <memory>
#include <string>

#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "VulkanMesh.h"

class VulkanTexture;
class VulkanGraphicsPipeline;

class VulkanPrefab {
public:
    VulkanPrefab() = default;

    ~VulkanPrefab() { Destroy(); }

    VulkanPrefab(
        const VulkanMesh                       *mesh,
        const VulkanTexture                    *baseTexture,
        const VulkanTexture                    *normalMap,
        const VulkanTexture                    *ormTexture,
        const VulkanTexture                    *emissiveTexture,
        const VulkanTexture                    *brdfTexture,
        const VulkanTexture                    *skyboxSpecular,
        const VulkanTexture                    *skyboxIrradiance,
        std::shared_ptr<VulkanGraphicsPipeline> pipeline
    );

    VulkanPrefab(
        const VulkanMesh                       *mesh,
        const VulkanTexture                    *baseTexture,
        const VulkanTexture                    *normalMap,
        const VulkanTexture                    *ormTexture,
        const VulkanTexture                    *emissiveTexture,
        const VulkanTexture                    *brdfTexture,
        const VulkanTexture                    *skyboxSpecular,
        const VulkanTexture                    *skyboxIrradiance,
        std::shared_ptr<VulkanGraphicsPipeline> pipeline,
        glm::vec3                               location
    );

    VulkanPrefab(
        const VulkanMesh                       *mesh,
        const VulkanTexture                    *baseTexture,
        const VulkanTexture                    *normalMap,
        const VulkanTexture                    *ormTexture,
        const VulkanTexture                    *emissiveTexture,
        const VulkanTexture                    *brdfTexture,
        const VulkanTexture                    *skyboxSpecular,
        const VulkanTexture                    *skyboxIrradiance,
        std::shared_ptr<VulkanGraphicsPipeline> pipeline,
        glm::vec3                               location,
        glm::vec3                               pitchYawRoll,
        glm::vec3                               scale
    );

    VulkanPrefab(
        const VulkanMesh                       *mesh,
        const VulkanTexture                    *baseTexture,
        const VulkanTexture                    *normalMap,
        const VulkanTexture                    *ormTexture,
        const VulkanTexture                    *emissiveTexture,
        const VulkanTexture                    *brdfTexture,
        const VulkanTexture                    *skyboxSpecular,
        const VulkanTexture                    *skyboxIrradiance,
        std::shared_ptr<VulkanGraphicsPipeline> pipeline,
        glm::vec3                               location,
        glm::quat                               rotation,
        glm::vec3                               scale
    );


    VulkanPrefab(const VulkanPrefab &) = delete;

    VulkanPrefab &operator=(const VulkanPrefab &) = delete;

    VulkanPrefab(VulkanPrefab &&other) { Swap(other); }

    VulkanPrefab &operator=(VulkanPrefab &&other) noexcept {
        if (this != &other) {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Destroy();

    void Swap(VulkanPrefab &other) noexcept;

    void SetLocation(glm::vec3 location);

    void SetScale(glm::vec3 scale);

    void SetRotation(glm::quat rotation);

    void SetRotation(glm::vec3 pitchYawRoll);

    void Reset();

    void BindAndDraw(VkCommandBuffer cmdBuf) const;

    [[nodiscard]] const std::string GetName() const { return m_mesh->GetName(); }

    [[nodiscard]] const glm::mat4 GetTransformation() const { return m_transformation; }

    [[nodiscard]] const VulkanMesh *GetMesh() const { return m_mesh; }

    [[nodiscard]] const glm::vec3 GetLocation() const { return m_location; }

    [[nodiscard]] const glm::vec3 GetScale() const { return m_scale; }

    [[nodiscard]] const glm::quat GetRotation() const { return m_rotation; }

    [[nodiscard]] const glm::vec3 GetPitchYawRoll() const { return m_pitchYawRoll; }

private:
    const VulkanMesh    *m_mesh            = nullptr;
    const VulkanTexture *m_baseTexture     = nullptr;
    const VulkanTexture *m_normalMap       = nullptr;
    const VulkanTexture *m_ormTexture      = nullptr;
    const VulkanTexture *m_emissiveTexture = nullptr;

    const VulkanTexture *m_brdfTexture      = nullptr;
    const VulkanTexture *m_skyboxSpecular   = nullptr;
    const VulkanTexture *m_skyboxIrradiance = nullptr;

    glm::mat4 m_transformation = glm::mat4(1.0f);

    glm::vec3 m_location     = glm::vec3(0.0f);
    glm::vec3 m_scale        = glm::vec3(1.0f);
    glm::quat m_rotation     = glm::quat();
    glm::vec3 m_pitchYawRoll = glm::vec3(1.0f); // In radians

    glm::vec3 m_originalLocation = glm::vec3(0.0f);

    std::shared_ptr<VulkanGraphicsPipeline> m_pipeline;

    std::vector<VkDescriptorSet> m_descriptorSets;

    void UpdateTransformation();

    void CreateDescriptorSets();

    VkDescriptorSet CreateDescriptorSet(const VkDescriptorSetLayout &layout);

    void UpdatePBRDescriptorSet();

    void UpdateIBLDescriptorSet();
};
