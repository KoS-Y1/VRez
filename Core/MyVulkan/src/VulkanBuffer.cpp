#include "include/VulkanBuffer.h"

#include <Debug.h>

#include <include/VulkanState.h>
#include <include/VulkanUtil.h>

VulkanBuffer::VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage) {
    CreateBuffer(size, usage);
    BindMemory();
}

void VulkanBuffer::Swap(VulkanBuffer &other) noexcept {
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_memory, other.m_memory);
}

void VulkanBuffer::Destroy() {
    if (m_buffer != VK_NULL_HANDLE) {
        vkFreeMemory(VulkanState::GetInstance().GetDevice(), m_memory, nullptr);
        vkDestroyBuffer(VulkanState::GetInstance().GetDevice(), m_buffer, nullptr);
    }

    m_buffer = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
}

void VulkanBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage) {
    VkBufferCreateInfo infoBuffer{
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .size                  = size,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
    };

    DEBUG_VK_ASSERT(vkCreateBuffer(VulkanState::GetInstance().GetDevice(), &infoBuffer, nullptr, &m_buffer));
}

void VulkanBuffer::BindMemory() {
    // Get buffer requirement first
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(VulkanState::GetInstance().GetDevice(), m_buffer, &memoryRequirements);

    // Get memory type index

    VkMemoryAllocateInfo infoMem{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = vk_util::FindMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        ),
    };

    DEBUG_VK_ASSERT(vkAllocateMemory(VulkanState::GetInstance().GetDevice(), &infoMem, nullptr, &m_memory));
    DEBUG_VK_ASSERT(vkBindBufferMemory(VulkanState::GetInstance().GetDevice(), m_buffer, m_memory, 0));
}

void VulkanBuffer::Upload(size_t size, const void *data) {
    void *mappedData;
    DEBUG_VK_ASSERT(vkMapMemory(VulkanState::GetInstance().GetDevice(), m_memory, 0, VK_WHOLE_SIZE, 0, &mappedData));
    memcpy(mappedData, data, size);
    vkUnmapMemory(VulkanState::GetInstance().GetDevice(), m_memory);
}