#pragma once

#include <string>

#include <glm/glm.hpp>

#include "VulkanMesh.h"

class VulkanMesh;

class MeshInstance
{
public:
    MeshInstance() = delete;

    ~MeshInstance() { Destroy(); }

    explicit MeshInstance(VulkanMesh *mesh) : m_mesh(mesh), m_model(glm::mat4(1.0f)) {}

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

    void Translate(const glm::vec3 &translation);

    void Scale(const glm::vec3 &scale);

    void Reset();

    [[nodiscard]] const std::string GetName() const { return m_mesh->GetName(); }
    [[nodiscard]] const glm::mat4 GetModel() const { return m_model; }

private:
    VulkanMesh *m_mesh;
    glm::mat4 m_model;
};
