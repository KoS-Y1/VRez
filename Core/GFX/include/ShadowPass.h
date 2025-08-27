#pragma once

#include "RenderPass.h"

class ShadowPass : public RenderPass {
public:
    ShadowPass();

    ~ShadowPass();

    ShadowPass(const ShadowPass &)            = delete;
    ShadowPass(ShadowPass &&)                 = delete;
    ShadowPass &operator=(const ShadowPass &) = delete;
    ShadowPass &operator=(ShadowPass &&)      = delete;

    void PreRender();
    void PostRender();

    [[nodiscard]] const VkDescriptorSet &GetCSMSet() const { return m_shadowSet; }

private:
    VulkanImage               m_shadowMap;
    VkRenderingAttachmentInfo m_shadowAttachment;
    VkDescriptorSet           m_shadowSet;
    VkSampler                 m_sampler;

    void CreateShadowMapImage();
    void CreateCSMSet();

    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
};