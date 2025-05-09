#pragma once
#include <vulkan/vulkan_core.h>

#include "VulkanState.h"


namespace vk_util
{
    void CmdImageLayoutTransition(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout,
                                  VkImageLayout newLayout, VkImageAspectFlags aspect, VkAccessFlags srcAccess,
                                  VkAccessFlags dstAccess);

    VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspect);
}
