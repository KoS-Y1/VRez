#pragma once

#include <vector>

#include "RenderPass.h"
struct DrawContent;

class GBufferPass : public RenderPass {
public:
    GBufferPass();

    ~GBufferPass();

    GBufferPass(const GBufferPass &)            = delete;
    GBufferPass(GBufferPass &&)                 = delete;
    GBufferPass &operator=(const GBufferPass &) = delete;
    GBufferPass &operator=(GBufferPass &&)      = delete;

    void PreRender();
    void PostRender();

    [[nodiscard]] const VkDescriptorSet &GetGBufferSet() const { return m_gBufferSet; }

private:
    std::vector<VulkanImage>               m_gBufferImages;
    std::vector<VkRenderingAttachmentInfo> m_gBufferAttachments;
    VkDescriptorSet                        m_gBufferSet = VK_NULL_HANDLE;
    VkSampler                              m_sampler = VK_NULL_HANDLE;

    void CreateGBufferImages();
    void CreateGBufferSet();

    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
};