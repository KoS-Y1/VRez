#pragma once

class VulkanMesh
{
public:
    VulkanMesh() = default;

    ~VulkanMesh() { Destroy(); }

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
    VulkanBuffer m_stagingBuffer;
    VulkanBuffer m_vertexBuffer;
    VulkanBuffer m_indexBuffer;

    VkDevice m_device = VK_NULL_HANDLE;
};