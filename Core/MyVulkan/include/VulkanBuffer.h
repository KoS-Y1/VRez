#pragma once

#include <vulkan/vulkan.h>

class VulkanBuffer
{
public:
    VulkanBuffer() = default;

    VulkanBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);

    ~VulkanBuffer() { Destroy(); }

    VulkanBuffer(const VulkanBuffer &) = delete;

    VulkanBuffer(VulkanBuffer &&other) noexcept { Swap(other); }

    VulkanBuffer &operator=(const VulkanBuffer &) = delete;

    VulkanBuffer &operator=(VulkanBuffer &&other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Swap(VulkanBuffer &other) noexcept;

    void Destroy();

    void Upload(size_t size, const void *data);

    [[nodiscard]] const VkBuffer &GetBuffer() const { return m_buffer; }

private:
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

    void BindMemory(VkPhysicalDevice physicalDevice);
};
