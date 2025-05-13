#pragma once

#include <vulkan/vulkan.h>

#define IMG_FORMAT VK_FORMAT_R16G16B16A16_SFLOAT


class VulkanImage
{
public:
    VulkanImage() = default;

    // Create both image and image view
    VulkanImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent,
                VkImageAspectFlags aspect);


    ~VulkanImage() { Destroy(); };

    // Disallow copy constructor
    VulkanImage(const VulkanImage &) = delete;


    VkImage GetImage() { return image; }
    VkImageView GetImageView() { return view; }

private:
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkFormat m_format = VK_FORMAT_UNDEFINED;

    void CreateImage(VkImageUsageFlags usage, VkExtent3D extent);

    void CreateImageView(VkImageAspectFlags aspect);

    void Destroy();
};
