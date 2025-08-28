#include "include/LightingPass.h"

#include <include/PbrRenderer.h>

#include <include/VulkanUtil.h>

void LightingPass::CreateRenderingInfo(const RenderingConfig &config, const DrawContent &content) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, &content.drawAttachments, nullptr);
    m_viewport = config.viewport;
}

void LightingPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    content.screen->BindAndDraw();
}
