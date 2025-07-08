#include "include/VulkanMesh.h"

VulkanMesh::VulkanMesh(VulkanState &state, std::string name, size_t vertexCount, size_t vertexSize, const void *data)
{
    VkDeviceSize size = vertexCount * vertexSize;
    VulkanBuffer stagingBuffer(state.GetPhysicalDevice(), state.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    stagingBuffer.Upload(size, data);

    VulkanBuffer vertexBuffer(state.GetPhysicalDevice(), state.GetDevice(), size,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    // Copy data from staging buffer to vertex buffer
    state.ImmediateSubmit([size, &stagingBuffer, &vertexBuffer](VkCommandBuffer cmdBuf)
    {
        VkBufferCopy copy
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
        };
        vkCmdCopyBuffer(cmdBuf, stagingBuffer.GetBuffer(), vertexBuffer.GetBuffer(), 1, &copy);
    });
    m_vertexBuffer = std::move(vertexBuffer);
    m_vertexCount = vertexCount;

    m_name = name;
}

void VulkanMesh::Swap(VulkanMesh &other)
{
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_vertexCount, other.m_vertexCount);
}

void VulkanMesh::Destroy()
{
    m_vertexBuffer.Destroy();
    m_vertexCount = 0;
}
