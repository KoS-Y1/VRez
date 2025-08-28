#include "include/ForwardPass.h"

#include "include/PbrRenderer.h"
#include "include/VulkanUtil.h"

void ForwardPass::CreateRenderingInfo(const RenderingConfig &config) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, &config.drawAttachments, &config.depthAttachments);
    m_viewport = config.viewport;
}

void ForwardPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    for (const auto& prefab : content.frontPrefabs) {
        prefab.BindAndDraw(layout);
    }
}
