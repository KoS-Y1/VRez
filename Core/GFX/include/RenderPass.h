#pragma once

#include "include/VulkanGraphicsPipeline.h"


#include <include/VulkanState.h>

struct RenderingConfig {
    VkRect2D                  renderArea;
    VkViewport                viewport;
    VkRenderingAttachmentInfo drawAttachments;
    VkRenderingAttachmentInfo depthAttachments;
};

struct DrawContent;
class VulkanGraphicsPipeline;

class RenderPass {
public:
    void Render(
        const RenderingConfig                                   &config,
        const std::vector<std::pair<VkDescriptorSet, uint32_t>> &globalSets,
        const DrawContent                                       &content,
        VulkanGraphicsPipeline                                  *pipeline
    ) {
        PreRender();

        CreateRenderingInfo(config);

        vkCmdBeginRendering(VulkanState::GetInstance().GetCommandBuffer(), &m_infoRendering);
        vkCmdSetViewport(VulkanState::GetInstance().GetCommandBuffer(), 0, 1, &config.viewport);
        vkCmdSetScissor(VulkanState::GetInstance().GetCommandBuffer(), 0, 1, &config.renderArea);

        Bind(globalSets, pipeline);

        DrawCalls(content, pipeline->GetLayout());

        vkCmdEndRendering(VulkanState::GetInstance().GetCommandBuffer());

        PostRender();
    }

protected:
    VkRenderingInfo m_infoRendering = {};

    RenderPass()  = default;
    ~RenderPass() = default;

    virtual void CreateRenderingInfo(const RenderingConfig &config)             = 0;
    virtual void DrawCalls(const DrawContent &content, VkPipelineLayout layout) = 0;
    virtual void PreRender()                                                    = 0;
    virtual void PostRender()                                                   = 0;

private:
    void Bind(const std::vector<std::pair<VkDescriptorSet, uint32_t>> &globalSets, VulkanGraphicsPipeline *pipeline) {
        vkCmdBindPipeline(VulkanState::GetInstance().GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

        for (const auto &pair: globalSets) {
            vkCmdBindDescriptorSets(
                VulkanState::GetInstance().GetCommandBuffer(),
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline->GetLayout(),
                pair.second,
                1,
                &pair.first,
                0,
                nullptr
            );
        }
    }
};