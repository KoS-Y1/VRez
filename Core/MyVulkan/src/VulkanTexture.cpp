#include "include/VulkanTexture.h"

#include <algorithm>

#include <Debug.h>

#include "include/VulkanState.h"
#include "include/VulkanBuffer.h"
// TODO:
// Get image data
// Upload data to a temporary stage buffer
// Immediate submit commands that copys data from buffer to image
VulkanTexture::VulkanTexture(VulkanState *state, uint32_t width, uint32_t height, const void *data,
                             SamplerConfig config)
{
    m_device = state->GetDevice();

    CreateImage(state, width, height, data);
    CreateSampler(config);
}

void VulkanTexture::Destroy()
{
    vkDestroySampler(m_device, m_sampler, nullptr);
    m_image.Destroy();
}

void VulkanTexture::Swap(VulkanTexture &other)
{
    std::swap(m_image, other.m_image);
    std::swap(m_sampler, other.m_sampler);
    std::swap(m_device, other.m_device);
}


void VulkanTexture::CreateImage(VulkanState *state, uint32_t width, uint32_t height, const void *data)
{
    VkExtent3D extent
    {
        .width = width,
        .height = height,
        .depth = 1,
    };
    VulkanImage img(state->GetPhysicalDevice(), m_device, COLOR_IMG_FORMAT,
                    VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, extent, VK_IMAGE_ASPECT_COLOR_BIT);
    m_image = std::move(img);

    VkDeviceSize size = width * height * COLOR_FORMAT_SIZE;

    VulkanBuffer stageBuffer(state->GetPhysicalDevice(), m_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    stageBuffer.Upload(size, data);

    state->ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
    {
        
    });
}

void VulkanTexture::CreateSampler(SamplerConfig config)
{
    VkSamplerCreateInfo infoSampler
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = config.filter,
        .minFilter = config.filter,
        .mipmapMode = config.mipmapMode,
        .addressModeU = config.addressMode,
        .addressModeV = config.addressMode,
        .addressModeW = config.addressMode,
        .mipLodBias = 0.0f,
        .anisotropyEnable = config.anisotropyEnable,
        .maxAnisotropy = config.maxAnisotropy,
        .compareEnable = config.compareEnable,
        .compareOp = config.compareOp,
        .minLod = config.minLod,
        .maxLod = config.maxLod,
        .borderColor = config.borderColor,
        .unnormalizedCoordinates = VK_FALSE, // Always normalized

    };

    DEBUG_VK_ASSERT(vkCreateSampler(m_device, &infoSampler, nullptr, &m_sampler));
}
