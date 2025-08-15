#pragma once

#include <string>

#include <include/VulkanBuffer.h>

class VulkanMesh {
public:
    VulkanMesh() = default;

    VulkanMesh(std::string name, size_t vertexCount, size_t vertexSize, const void *data);

    ~VulkanMesh() { Destroy(); }

    VulkanMesh(const VulkanMesh &) = delete;

    VulkanMesh &operator=(const VulkanMesh &) = delete;

    VulkanMesh(VulkanMesh &&other) noexcept { Swap(other); }

    VulkanMesh &operator=(VulkanMesh &&other) noexcept {
        if (this != &other) {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Swap(VulkanMesh &other) noexcept;

    void Destroy();

    [[nodiscard]] const VkBuffer &GetVertexBuffer() const { return m_vertexBuffer.GetBuffer(); }

    [[nodiscard]] const size_t GetVertexCount() const { return m_vertexCount; }

    [[nodiscard]] const std::string GetName() const { return m_name; }

private:
    VulkanBuffer m_vertexBuffer;
    size_t       m_vertexCount = 0;
    size_t       m_vertexSize  = 0;
    std::string  m_name;
};
