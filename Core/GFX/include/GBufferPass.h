#pragma once

#include <vector>

#include "RenderPass.h"
struct DrawContent;

class GBufferPass : public RenderPass {
public:
    GBufferPass();

    ~GBufferPass() { Destroy(); }

    GBufferPass(const GBufferPass &)            = delete;
    GBufferPass &operator=(const GBufferPass &) = delete;

    GBufferPass(GBufferPass &&other) { Swap(other); }

    GBufferPass &operator=(GBufferPass &&other) {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void PreRender();
    void PostRender();

    void Swap(GBufferPass &other) noexcept;

    void Destroy();

    [[nodiscard]] const VkDescriptorSet &GetGBufferSet() const { return m_gBufferSet; }

private:
    std::vector<VulkanImage>               m_gBufferImages;
    std::vector<VkRenderingAttachmentInfo> m_gBufferAttachments;
    VkDescriptorSet                        m_gBufferSet;

    void CreateGBufferImages();
    void CreateGBufferSet();

    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;

};