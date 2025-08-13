#include "include/VulkanImage.h"

#include <Debug.h>
#include <format>

#include "include/VulkanUtil.h"

VulkanImage::VulkanImage(
    VkPhysicalDevice      physicalDevice,
    VkDevice              device,
    VkFormat              format,
    VkImageUsageFlags     usage,
    VkExtent3D            extent,
    VkImageAspectFlags    aspect,
    VkSampleCountFlagBits samples
) {
    m_device = device;
    m_format = format;
    m_extent = extent;
    CreateImage(usage, extent, samples);
    BindMemory(physicalDevice);
    CreateImageView(aspect);
}

void VulkanImage::Destroy() {
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyImageView(m_device, m_view, nullptr);
        vkFreeMemory(m_device, m_memory, nullptr);
        vkDestroyImage(m_device, m_image, nullptr);
    }

    m_device = VK_NULL_HANDLE;
    m_image  = VK_NULL_HANDLE;
    m_view   = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
}

void VulkanImage::CreateImage(VkImageUsageFlags usage, VkExtent3D extent, VkSampleCountFlagBits samples) {
    VkImageCreateInfo infoImage{
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .imageType             = VK_IMAGE_TYPE_2D,
        .format                = m_format,
        .extent                = extent,
        .mipLevels             = 1,
        .arrayLayers           = 1,
        .samples               = samples,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    DEBUG_VK_ASSERT(vkCreateImage(m_device, &infoImage, nullptr, &m_image));
}

void VulkanImage::CreateImageView(VkImageAspectFlags aspect) {
    VkImageViewCreateInfo infoView{
        .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext      = nullptr,
        .flags      = 0,
        .image      = m_image,
        .viewType   = VK_IMAGE_VIEW_TYPE_2D,
        .format     = m_format,
        .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = vk_util::GetSubresourceRange(aspect)
    };

    DEBUG_VK_ASSERT(vkCreateImageView(m_device, &infoView, nullptr, &m_view));
}

void VulkanImage::BindMemory(VkPhysicalDevice physicalDevice) {
    // Get image requirement first
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, m_image, &memRequirements);

    VkMemoryAllocateInfo infoMem{
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memRequirements.size,
        .memoryTypeIndex = vk_util::FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };

    DEBUG_VK_ASSERT(vkAllocateMemory(m_device, &infoMem, nullptr, &m_memory));
    DEBUG_VK_ASSERT(vkBindImageMemory(m_device, m_image, m_memory, 0));
}

void VulkanImage::Swap(VulkanImage &other) noexcept {
    m_device = other.m_device;
    m_extent = other.m_extent;
    m_format = other.m_format;

    std::swap(m_image, other.m_image);
    std::swap(m_view, other.m_view);
    std::swap(m_memory, other.m_memory);
}
