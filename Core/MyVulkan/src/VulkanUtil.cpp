#include <include/VulkanUtil.h>
#include <SDL3/SDL.h>

void vk_util::CmdImageLayoutTransition(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout,
                                       VkImageLayout newLayout, VkImageAspectFlags aspect, VkAccessFlags srcAccess,
                                       VkAccessFlags dstAccess)
{
    VkImageMemoryBarrier barrier
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = srcAccess,
        .dstAccessMask = dstAccess,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image = image,
        .subresourceRange = GetSubresourceRange(aspect),
    };

    vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0,
                         nullptr, 0, nullptr, 1,
                         &barrier);
}

VkImageSubresourceRange vk_util::GetSubresourceRange(VkImageAspectFlags aspect)
{
    VkImageSubresourceRange subresourceRange
    {
        .aspectMask = aspect,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    return subresourceRange;
}

uint32_t vk_util::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBitsRequirement,
                                 VkMemoryPropertyFlags requiredProperties)
{
    // Get physical device properties first
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; memoryIndex++)
    {
        const uint32_t memoryTypeBits = (1 << memoryIndex);
        const bool isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;

        const VkMemoryPropertyFlags properties = memoryProperties.memoryTypes[memoryIndex].propertyFlags;
        const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

        if (isRequiredMemoryType && hasRequiredProperties)
        {
            return memoryIndex;
        }
    }

    SDL_Log("Failed to find suitable memory type for typeBits %d, properties %d!", memoryTypeBitsRequirement,
            requiredProperties);
    return -1;
}

void vk_util::CopyImageToImage(VkCommandBuffer cmdBuf, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent,
                               VkExtent2D dstExtent, VkImageAspectFlags aspect)
{
    VkImageBlit blit
    {
        
    };
}

VkImageSubresourceLayers vk_util::GetImageSubresourceLayers(VkImageAspectFlags aspect)
{
    VkImageSubresourceLayers subresourceLayers
    {
        .aspectMask = aspect,
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    return subresourceLayers;
}
