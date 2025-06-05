#include "include/VulkanMesh.h"

VulkanMesh::VulkanMesh(VulkanState &state, size_t count, size_t size, const void *data)
{
    VulkanBuffer stagingBuffer(state.GetPhysicalDevice(), state.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    stagingBuffer.Upload(size, data);

    VulkanBuffer vertexBuffer(state.GetPhysicalDevice(), state.GetDevice(), size,
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
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
    m_vertexCount = count;
}
