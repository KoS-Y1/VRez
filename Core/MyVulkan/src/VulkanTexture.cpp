#include "include/VulkanTexture.h"

#include <algorithm>

#include <Debug.h>

#include <include/VulkanBuffer.h>
#include <include/VulkanState.h>
#include <include/VulkanUtil.h>

VulkanTexture::VulkanTexture(
    VulkanState        &state,
    uint32_t            width,
    uint32_t            height,
    VkFormat            format,
    size_t              formatSize,
    const void         *data,
    const SamplerConfig config
) {
    m_device = state.GetDevice();

    CreateImage(state, width, height, format, formatSize, data);
    CreateSampler(config);
}

void VulkanTexture::Destroy() {
    if (m_device != VK_NULL_HANDLE) {
        vkDestroySampler(m_device, m_sampler, nullptr);
        m_image.Destroy();

        m_device = VK_NULL_HANDLE;
    }
}

void VulkanTexture::Swap(VulkanTexture &other) noexcept {
    std::swap(m_image, other.m_image);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_device, other.m_device);
}

void VulkanTexture::CreateImage(VulkanState &state, uint32_t width, uint32_t height, VkFormat format, size_t formatSize, const void *data) {
    VkExtent3D extent{
        .width  = width,
        .height = height,
        .depth  = 1,
    };
    VulkanImage img(
        state.GetPhysicalDevice(),
        m_device,
        format,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        extent,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    m_image = std::move(img);

    VkDeviceSize size = width * height * formatSize;

    VulkanBuffer stagingBuffer(state.GetPhysicalDevice(), m_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    stagingBuffer.Upload(size, data);

    state.ImmediateSubmit([&](VkCommandBuffer cmdBuf) {
        // Layout transition
        vk_util::CmdImageLayoutTransition(
            cmdBuf,
            m_image.GetImage(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT
        );

        VkBufferImageCopy copy{
            .bufferOffset      = 0,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource  = vk_util::GetImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT),
            .imageOffset       = 0,
            .imageExtent       = extent
        };

        vkCmdCopyBufferToImage(cmdBuf, stagingBuffer.GetBuffer(), m_image.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

        // Layout transition
        vk_util::CmdImageLayoutTransition(
            cmdBuf,
            m_image.GetImage(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_WRITE_BIT
        );
    });
}

void VulkanTexture::CreateSampler(SamplerConfig config) {
    VkSamplerCreateInfo infoSampler{
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .magFilter               = config.filter,
        .minFilter               = config.filter,
        .mipmapMode              = config.mipmapMode,
        .addressModeU            = config.addressMode,
        .addressModeV            = config.addressMode,
        .addressModeW            = config.addressMode,
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = config.anisotropyEnable,
        .maxAnisotropy           = config.maxAnisotropy,
        .compareEnable           = config.compareEnable,
        .compareOp               = config.compareOp,
        .minLod                  = config.minLod,
        .maxLod                  = config.maxLod,
        .borderColor             = config.borderColor,
        .unnormalizedCoordinates = VK_FALSE, // Always normalized

    };

    DEBUG_VK_ASSERT(vkCreateSampler(m_device, &infoSampler, nullptr, &m_sampler));
}
