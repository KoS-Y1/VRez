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
        CreateRenderingInfo(config);

        vkCmdBeginRendering(VulkanState::GetInstance().GetCommandBuffer(), &m_infoRendering);
        vkCmdSetViewport(VulkanState::GetInstance().GetCommandBuffer(), 0, 1, &m_viewport);
        vkCmdSetScissor(VulkanState::GetInstance().GetCommandBuffer(), 0, 1, &m_infoRendering.renderArea);

        Bind(globalSets, pipeline);

        DrawCalls(content, pipeline->GetLayout());

        vkCmdEndRendering(VulkanState::GetInstance().GetCommandBuffer());
    }

protected:
    VkRenderingInfo m_infoRendering = {};
    VkViewport m_viewport = {};

    RenderPass()  = default;
    ~RenderPass() = default;

    virtual void CreateRenderingInfo(const RenderingConfig &config)             = 0;
    virtual void DrawCalls(const DrawContent &content, VkPipelineLayout layout) = 0;

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