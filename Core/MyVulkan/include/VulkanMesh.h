#pragma once

#include "include/VulkanState.h"
#include "include/VulkanBuffer.h"

class VulkanMesh
{
public:
    VulkanMesh() = default;

    VulkanMesh(VulkanState &state, size_t vertexCount, size_t vertexSize, const void *data);

    ~VulkanMesh() { Destroy(); }

    VulkanMesh(const VulkanMesh &) = delete;

    VulkanMesh &operator=(const VulkanMesh &) = delete;

    VulkanMesh(VulkanMesh &&other) noexcept { Swap(other); }

    VulkanMesh &operator=(VulkanMesh &&other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Swap(VulkanMesh &other);

    void Destroy();

    [[nodiscard]] VkBuffer const &GetVertexBuffer() const { return m_vertexBuffer.GetBuffer(); }
    [[nodiscard]] size_t const GetVertexCount() const { return m_vertexCount; }

private:
    VulkanBuffer m_vertexBuffer;
    size_t m_vertexCount;
};
