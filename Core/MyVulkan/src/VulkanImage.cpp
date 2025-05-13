#include "include/VulkanImage.h"

#include <Assert.h>
#include <format>

#include "include/VulkanUtil.h"

VulkanImage::VulkanImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent,
                         VkImageAspectFlags aspect)
{
    m_device = device;
    m_format = format;

    CreateImage(usage, extent);
    CreateImageView(aspect);
}

void VulkanImage::Destroy()
{
    vkDestroyImage(m_device, image, nullptr);
    vkDestroyImageView(m_device, view, nullptr);
}


void VulkanImage::CreateImage(VkImageUsageFlags usage, VkExtent3D extent)
{
    VkImageCreateInfo infoImage
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = m_format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    DEBUG_VK_ASSERT(vkCreateImage(m_device, &infoImage, nullptr, &image));
}

void VulkanImage::CreateImageView(VkImageAspectFlags aspect)
{
    VkImageViewCreateInfo infoView
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = m_format,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = vk_util::GetSubresourceRange(aspect)
    };

    DEBUG_VK_ASSERT(vkCreateImageView(m_device, &infoView, nullptr, &view));
}
