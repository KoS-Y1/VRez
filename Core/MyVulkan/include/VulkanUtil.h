#pragma once
#include <vector>

#include <vulkan/vulkan_core.h>

namespace vk_util {
void CmdImageLayoutTransition(
    VkCommandBuffer    cmdBuf,
    VkImage            image,
    VkImageLayout      oldLayout,
    VkImageLayout      newLayout,
    VkImageAspectFlags aspect,
    VkAccessFlags      srcAccess,
    VkAccessFlags      dstAccess,
    uint32_t           baseLevel   = 0,
    uint32_t           levelCount  = 1,
    uint32_t           baseLayer   = 0,
    uint32_t           arrayLayers = 1
);

VkImageSubresourceRange GetSubresourceRange(
    VkImageAspectFlags aspect,
    uint32_t           baseLevel  = 0,
    uint32_t           levelCount = 1,
    uint32_t           baseLayer  = 0,
    uint32_t           layerCount = 1
);

uint32_t FindMemoryType(uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties);

void CmdCopyImageToImage(
    VkCommandBuffer    cmdBuf,
    VkImage            srcImage,
    VkImage            dstImage,
    VkExtent3D         srcExtent,
    VkExtent3D         dstExtent,
    VkImageAspectFlags aspect
);

VkImageSubresourceLayers GetImageSubresourceLayers(
    VkImageAspectFlags aspect,
    uint32_t           baseLevel  = 0,
    uint32_t           baseLayer  = 0,
    uint32_t           layerCount = 1
);


VkRenderingAttachmentInfo GetRenderingAttachmentInfo(
    VkImageView           view,
    VkImageLayout         layout,
    VkClearValue         *clear,
    VkAttachmentLoadOp    loadOp,
    VkAttachmentStoreOp   storeOp,
    VkResolveModeFlagBits resolveMode,
    VkImageView           resolveView,
    VkImageLayout         resolveLayout
);

VkRenderingInfo GetRenderingInfo(
    VkRect2D                                      area,
    const std::vector<VkRenderingAttachmentInfo> &colorAttachments,
    const VkRenderingAttachmentInfo              *depthStencilAttachment,
    uint32_t                                      layerCount = 1
);

VkRenderingInfo GetRenderingInfo(
    VkRect2D                         area,
    const VkRenderingAttachmentInfo *colorAttachment,
    const VkRenderingAttachmentInfo *depthStencilAttachment,
    uint32_t                         layerCount = 1
);

VkDescriptorSet CreateDescriptorSet(const VkDescriptorSetLayout &layout);

void CmdBlitMipmap(VkCommandBuffer cmdBuf, VkImage image, VkExtent3D srcExtent, VkExtent3D dstExtent, VkImageAspectFlags aspect, uint32_t baseLevel);
} // namespace vk_util
