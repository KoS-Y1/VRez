#include "include/ForwardPass.h"

#include "include/PbrRenderer.h"
#include "include/VulkanUtil.h"

void ForwardPass::CreateRenderingInfo(const RenderingConfig &config, const DrawContent &content) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, &content.drawAttachments, &content.depthAttachments);
    m_viewport = config.viewport;
}

void ForwardPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    for (const auto& prefab : content.frontPrefabs) {
        prefab.BindAndDraw(layout);
    }
}
