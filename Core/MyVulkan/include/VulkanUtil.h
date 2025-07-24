#pragma once
#include <string>
#include <vulkan/vulkan_core.h>

#include "VulkanState.h"


namespace vk_util
{
    void CmdImageLayoutTransition(VkCommandBuffer cmdBuf, VkImage image, VkImageLayout oldLayout,
                                  VkImageLayout newLayout, VkImageAspectFlags aspect, VkAccessFlags srcAccess,
                                  VkAccessFlags dstAccess);

    VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspect);

    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags
                            requiredProperties);

    void CopyImageToImage(VkCommandBuffer cmdBuf, VkImage srcImage, VkImage dstImage, VkExtent3D srcExtent,
                          VkExtent3D dstExtent, VkImageAspectFlags aspect);

    VkImageSubresourceLayers GetImageSubresourceLayers(VkImageAspectFlags aspect);


    VkRenderingAttachmentInfo GetRenderingAttachmentInfo(VkImageView view, VkImageLayout layout, VkClearValue *clear,
                                                         VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp);

    VkRenderingInfo GetRenderingInfo(VkRect2D area, VkRenderingAttachmentInfo *colorAttachment,
                                     VkRenderingAttachmentInfo *depthStencilAttachment);

}
