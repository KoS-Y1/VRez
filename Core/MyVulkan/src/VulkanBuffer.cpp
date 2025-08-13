#include "include/VulkanBuffer.h"

#include <Debug.h>

#include "include/VulkanUtil.h"

VulkanBuffer::VulkanBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage) {
    m_device = device;

    CreateBuffer(size, usage);
    BindMemory(physicalDevice);
}

void VulkanBuffer::Swap(VulkanBuffer &other) noexcept {
    m_device = other.m_device;

    std::swap(m_buffer, other.m_buffer);
    std::swap(m_memory, other.m_memory);
}

void VulkanBuffer::Destroy() {
    if (m_device != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_memory, nullptr);
        vkDestroyBuffer(m_device, m_buffer, nullptr);
    }

    m_device = VK_NULL_HANDLE;
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

    DEBUG_VK_ASSERT(vkCreateBuffer(m_device, &infoBuffer, nullptr, &m_buffer));
}

void VulkanBuffer::BindMemory(VkPhysicalDevice physicalDevice) {
    // Get buffer requirement first
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memoryRequirements);

    // Get memory type index

    VkMemoryAllocateInfo infoMem{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = vk_util::FindMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
    };

    DEBUG_VK_ASSERT(vkAllocateMemory(m_device, &infoMem, nullptr, &m_memory));
    DEBUG_VK_ASSERT(vkBindBufferMemory(m_device, m_buffer, m_memory, 0));
}

void VulkanBuffer::Upload(size_t size, const void *data) {
    void *mappedData;
    DEBUG_VK_ASSERT(vkMapMemory(m_device, m_memory, 0, VK_WHOLE_SIZE, 0, &mappedData));
    memcpy(mappedData, data, size);
    vkUnmapMemory(m_device, m_memory);
}