#include <include/VulkanUtil.h>

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
