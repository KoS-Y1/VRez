#pragma once

class VulkanBuffer
{
public:
    VulkanBuffer() = default;

    ~VulkanBuffer(){ Destroy(); }

    VulkanBuffer(const VulkanBuffer&) = delete;

    VulkanBuffer(VulkanBuffer&&) noexcept { Swap(other); }

    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VulkanBuffer& operator=(VulkanBuffer &&) noexcept
    {
        if (this != &other)
        {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Swap(VUlkanBuffer &other);

    void Destroy();

    [[nodiscard]] const VkBuffer &GetBuffer() const { return buffer; }

private:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
};