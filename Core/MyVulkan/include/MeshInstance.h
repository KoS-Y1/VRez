#pragma once

#include <string>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "VulkanMesh.h"

class VulkanMesh;

class MeshInstance
{
public:
    MeshInstance() = delete;

    ~MeshInstance() { Destroy(); }

    explicit MeshInstance(const VulkanMesh *mesh) : m_mesh(mesh), m_transformation(glm::mat4(1.0f))
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(m_transformation, m_scale, m_rotation, m_location, skew, perspective);
        m_pitchYawRoll = glm::eulerAngles(m_rotation);
    }

    MeshInstance(const MeshInstance &) = delete;

    MeshInstance &operator=(const MeshInstance &) = delete;

    MeshInstance(MeshInstance &&other) { Swap(other); }

    MeshInstance &operator=(MeshInstance &&other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Destroy();

    void Swap(MeshInstance &other) noexcept;

    void SetLocation(glm::vec3 location);

    void SetScale(glm::vec3 scale);

    void SetRotation(glm::quat rotation);

    void SetRotation(glm::vec3 pitchYawRoll);

    void Reset();

    [[nodiscard]] const std::string GetName() const { return m_mesh->GetName(); }
    [[nodiscard]] const glm::mat4 GetTransformation() const { return m_transformation; }
    [[nodiscard]] const VulkanMesh *GetMesh() const { return m_mesh; }
    [[nodiscard]] const glm::vec3 GetLocation() const { return m_location; }
    [[nodiscard]] const glm::vec3 GetScale() const { return m_scale; }
    [[nodiscard]] const glm::quat GetRotation() const { return m_rotation; }
    [[nodiscard]] const glm::vec3 GetPitchYawRoll() const { return m_pitchYawRoll; }

private:
    const VulkanMesh *m_mesh;
    glm::mat4 m_transformation;

    glm::vec3 m_location;
    glm::vec3 m_scale;
    glm::quat m_rotation;
    glm::vec3 m_pitchYawRoll;

    void UpdateTransformation();
};
