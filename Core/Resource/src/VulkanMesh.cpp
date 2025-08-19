#include "include/VulkanMesh.h"

#include "include/VulkanState.h"

VulkanMesh::VulkanMesh(std::string name, size_t vertexCount, size_t vertexSize, const void *data) {
    VkDeviceSize size = vertexCount * vertexSize;
    m_vertexSize      = vertexSize;
    VulkanBuffer stagingBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    stagingBuffer.Upload(size, data);

    VulkanBuffer vertexBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    // Copy data from staging buffer to vertex buffer
    VulkanState::GetInstance().ImmediateSubmit([size, &stagingBuffer, &vertexBuffer](VkCommandBuffer cmdBuf) {
        VkBufferCopy copy{.srcOffset = 0, .dstOffset = 0, .size = size};
        vkCmdCopyBuffer(cmdBuf, stagingBuffer.GetBuffer(), vertexBuffer.GetBuffer(), 1, &copy);
    });
    m_vertexBuffer = std::move(vertexBuffer);
    m_vertexCount  = vertexCount;

    m_name = name;
}

void VulkanMesh::Swap(VulkanMesh &other) noexcept {
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_vertexCount, other.m_vertexCount);
    std::swap(m_name, other.m_name);
}

void VulkanMesh::Destroy() {
    m_vertexBuffer.Destroy();
    m_vertexCount = 0;
}

void VulkanMesh::BindAndDraw() const {
    const VkDeviceSize offset = 0;

    vkCmdBindVertexBuffers(VulkanState::GetInstance().GetCommandBuffer(), 0, 1, &m_vertexBuffer.GetBuffer(), &offset);
    vkCmdDraw(VulkanState::GetInstance().GetCommandBuffer(), m_vertexCount, 1, 0, 0);
}