#include <SDL3/SDL.h>
#include <include/VulkanUtil.h>

void vk_util::CmdImageLayoutTransition(
    VkCommandBuffer    cmdBuf,
    VkImage            image,
    VkImageLayout      oldLayout,
    VkImageLayout      newLayout,
    VkImageAspectFlags aspect,
    VkAccessFlags      srcAccess,
    VkAccessFlags      dstAccess
) {
    VkImageMemoryBarrier barrier{
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = srcAccess,
        .dstAccessMask       = dstAccess,
        .oldLayout           = oldLayout,
        .newLayout           = newLayout,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image               = image,
        .subresourceRange    = GetSubresourceRange(aspect),
    };

    vkCmdPipelineBarrier(cmdBuf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

VkImageSubresourceRange vk_util::GetSubresourceRange(VkImageAspectFlags aspect) {
    VkImageSubresourceRange subresourceRange{
        .aspectMask     = aspect,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    return subresourceRange;
}

uint32_t vk_util::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties) {
    // Get physical device properties first
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; memoryIndex++) {
        const uint32_t memoryTypeBits       = (1 << memoryIndex);
        const bool     isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;

        const VkMemoryPropertyFlags properties            = memoryProperties.memoryTypes[memoryIndex].propertyFlags;
        const bool                  hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

        if (isRequiredMemoryType && hasRequiredProperties) {
            return memoryIndex;
        }
    }

    SDL_Log("Failed to find suitable memory type for typeBits %d, properties %d!", memoryTypeBitsRequirement, requiredProperties);
    return -1;
}

void vk_util::CopyImageToImage(
    VkCommandBuffer    cmdBuf,
    VkImage            srcImage,
    VkImage            dstImage,
    VkExtent3D         srcExtent,
    VkExtent3D         dstExtent,
    VkImageAspectFlags aspect
) {
    VkImageBlit blit{
        .srcSubresource = GetImageSubresourceLayers(aspect),
        .dstSubresource = GetImageSubresourceLayers(aspect),
    };
    blit.srcOffsets[1].x = static_cast<int32_t>(srcExtent.width);
    blit.srcOffsets[1].y = static_cast<int32_t>(srcExtent.height);
    blit.srcOffsets[1].z = static_cast<int32_t>(srcExtent.depth);
    blit.dstOffsets[1].x = static_cast<int32_t>(dstExtent.width);
    blit.dstOffsets[1].y = static_cast<int32_t>(dstExtent.height);
    blit.dstOffsets[1].z = static_cast<int32_t>(dstExtent.depth);

    vkCmdBlitImage(
        cmdBuf,
        srcImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dstImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &blit,
        VK_FILTER_LINEAR
    );
}

VkImageSubresourceLayers vk_util::GetImageSubresourceLayers(VkImageAspectFlags aspect) {
    VkImageSubresourceLayers subresourceLayers{
        .aspectMask     = aspect,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    return subresourceLayers;
}

VkRenderingAttachmentInfo vk_util::GetRenderingAttachmentInfo(
    VkImageView           view,
    VkImageLayout         layout,
    VkClearValue         *clear,
    VkAttachmentLoadOp    loadOp,
    VkAttachmentStoreOp   storeOp,
    VkResolveModeFlagBits resolveMode,
    VkImageView           resolveView,
    VkImageLayout         resolveLayout
) {
    VkRenderingAttachmentInfo infoAttachment{
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext              = nullptr,
        .imageView          = view,
        .imageLayout        = layout,
        .resolveMode        = resolveMode,
        .resolveImageView   = resolveView,
        .resolveImageLayout = resolveLayout,
        .loadOp             = loadOp,
        .storeOp            = storeOp,
    };

    if (clear != nullptr) {
        infoAttachment.loadOp     = VK_ATTACHMENT_LOAD_OP_CLEAR;
        infoAttachment.clearValue = *(clear);
    }

    return infoAttachment;
}

VkRenderingInfo vk_util::GetRenderingInfo(
    VkRect2D                   area,
    VkRenderingAttachmentInfo *colorAttachment,
    VkRenderingAttachmentInfo *depthStencilAttachment
) {
    VkRenderingInfo infoRendering{
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext                = nullptr,
        .flags                = 0,
        .renderArea           = area,
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments    = colorAttachment,
        .pDepthAttachment     = depthStencilAttachment,
        .pStencilAttachment   = nullptr,
    };
    return infoRendering;
}
