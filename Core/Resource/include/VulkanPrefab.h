#pragma once

#include <string>

#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "VulkanMesh.h"
#include "VulkanObject.h"

class VulkanObject;

class VulkanPrefab {
public:
    explicit VulkanPrefab(const std::string& key) : VulkanPrefab(key, glm::vec3(0.0f)) {}

    VulkanPrefab(const std::string& key, glm::vec3 location);

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

    void BindAndDraw(VkCommandBuffer cmdBuf, VkPipelineLayout pipeline) const;

    [[nodiscard]] const std::string GetName() const { return m_object->GetName(); }

    [[nodiscard]] const glm::vec3 GetLocation() const { return m_location; }

    [[nodiscard]] const glm::vec3 GetScale() const { return m_scale; }

    [[nodiscard]] const glm::vec3 GetPitchYawRoll() const { return m_pitchYawRoll; }

private:
    const VulkanObject *m_object;

    glm::mat4 m_transformation   = glm::mat4(1.0f);
    glm::vec3 m_location         = glm::vec3(0.0f);
    glm::vec3 m_scale            = glm::vec3(1.0f);
    glm::quat m_rotation         = glm::quat();
    glm::vec3 m_pitchYawRoll     = glm::vec3(1.0f); // In radians
    glm::vec3 m_originalLocation = glm::vec3(0.0f);

    void UpdateTransformation();
};
