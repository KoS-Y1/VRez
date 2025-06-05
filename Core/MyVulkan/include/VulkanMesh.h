#pragma once

#include "include/VulkanState.h"
#include "include/VulkanBuffer.h"

class VulkanMesh
{
public:
    VulkanMesh() = default;

    ~VulkanMesh() { Destroy(); }

    VulkanMesh(VulkanState &state, size_t count, size_t size, const void* data);

    VulkanMesh(const VulkanMesh&) = delete;
    VulkanMesh& operator=(const VulkanMesh&) = delete;

    VulkanMesh(VulkanMesh&& other) noexcept { Swap(other); }
    VulkanMesh& operator=(VulkanMesh&& other) noexcept
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

private:
    VulkanBuffer m_vertexBuffer;
    size_t m_vertexCount;

};