#include "include/VulkanImage.h"

#include <Debug.h>
#include <format>

#include "include/VulkanUtil.h"

VulkanImage::VulkanImage(VkPhysicalDevice physicalDevice, VkDevice device, VkFormat format, VkImageUsageFlags usage, VkExtent3D extent,
                         VkImageAspectFlags aspect)
{
    m_physicalDevice = physicalDevice;
    m_device = device;
    m_format = format;
    m_extent = extent;
    CreateImage(usage, extent);
    BindMemory();
    CreateImageView(aspect);
}

void VulkanImage::Destroy()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_device, view, nullptr);
        vkFreeMemory(m_device, memory, nullptr);
        vkDestroyImage(m_device, image, nullptr);
    }

    m_device = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    image = VK_NULL_HANDLE;
    view = VK_NULL_HANDLE;
    memory = VK_NULL_HANDLE;
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

void VulkanImage::BindMemory()
{

    // Get image requirement first
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memRequirements);

    // Get memory type index
    uint32_t memoryTypeIndex = vk_util::FindMemoryType(m_physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo infoMem
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = memoryTypeIndex,
    };

    DEBUG_VK_ASSERT(vkAllocateMemory(m_device, &infoMem, nullptr, &memory));
    DEBUG_VK_ASSERT(vkBindImageMemory(m_device, image, memory, 0));
}

void VulkanImage::Swap(VulkanImage &other) noexcept
{
    m_device = other.m_device;
    m_physicalDevice = other.m_physicalDevice;
    m_extent = other.m_extent;
    m_format = other.m_format;

    std::swap(image, other.image);
    std::swap(view, other.view);
    std::swap(memory, other.memory);
}
