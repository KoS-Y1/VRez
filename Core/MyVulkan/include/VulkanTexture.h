#pragma once

#include "VulkanImage.h"

class VulkanState;

// TODO: add more config if needed
struct SamplerConfig {
    VkFilter             filter           = VK_FILTER_LINEAR;
    VkSamplerMipmapMode  mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode addressMode      = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkBool32             anisotropyEnable = VK_FALSE;
    float                maxAnisotropy    = 1.0f;
    VkBool32             compareEnable    = VK_FALSE;
    VkCompareOp          compareOp        = VK_COMPARE_OP_ALWAYS;
    float                minLod           = 0.0f;
    float                maxLod           = 1.0f;
    VkBorderColor        borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
};

class VulkanTexture {
public:
    VulkanTexture() = default;

    ~VulkanTexture() { Destroy(); }

    VulkanTexture(
        VulkanState        &state,
        uint32_t            width,
        uint32_t            height,
        VkFormat            format,
        size_t              formatSize,
        const void         *data,
        const SamplerConfig config
    );

    VulkanTexture(const VulkanTexture &) = delete;

    VulkanTexture &operator=(const VulkanTexture &) = delete;

    VulkanTexture(VulkanTexture &&other) noexcept { Swap(other); }

    VulkanTexture &operator=(VulkanTexture &&other) noexcept {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void Destroy();

    void Swap(VulkanTexture &other) noexcept;

    [[nodiscard]] VkSampler GetSampler() const { return m_sampler; }

    [[nodiscard]] VkImageView GetImageView() const { return m_image.GetImageView(); }

private:
    VulkanImage m_image;
    VkDevice    m_device  = VK_NULL_HANDLE;
    VkSampler   m_sampler = VK_NULL_HANDLE;

    void CreateImage(VulkanState &state, uint32_t width, uint32_t height, VkFormat format, size_t formatSize, const void *data);

    void CreateSampler(SamplerConfig config);
};
