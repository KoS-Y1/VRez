#pragma once

#include <vulkan/vulkan.h>

#define IMG_FORMAT VK_FORMAT_R16G16B16A16_SFLOAT


class VulkanImage
{
public:
    VulkanImage() = default;

    // Create both image and image view
    VulkanImage(VkPhysicalDevice physicalDevice, VkDevice device, VkFormat format, VkImageUsageFlags usage,
                VkExtent3D extent,
                VkImageAspectFlags aspect);


    ~VulkanImage() { Destroy(); };

    VulkanImage(const VulkanImage &) = delete;

    VulkanImage(VulkanImage &&other) noexcept { Swap(other); };

    VulkanImage &operator=(const VulkanImage &) = delete;

    VulkanImage &operator=(VulkanImage &&other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            Swap(other);
        }
        return *this;
    }

    void Swap(VulkanImage &other) noexcept;

    void Destroy();

    [[nodiscard]] const VkImage &GetImage() const { return image; }
    [[nodiscard]] const VkImageView &GetImageView() const { return view; }
    [[nodiscard]] const VkExtent3D &GetExtent() const { return m_extent; }

private:
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkExtent3D m_extent = {0};
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkFormat m_format = VK_FORMAT_UNDEFINED;

    void CreateImage(VkImageUsageFlags usage, VkExtent3D extent);

    void CreateImageView(VkImageAspectFlags aspect);

    void BindMemory();
};
