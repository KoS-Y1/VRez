#include "include/PostProcessingPass.h"

#include "include/PbrRenderer.h"
#include "include/VulkanUtil.h"

void PostProcessingPass::CreateRenderingInfo(const RenderingConfig &config, const DrawContent &content) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, &content.postProcessdAttachments, nullptr);
    m_viewport = config.viewport;
}

void PostProcessingPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    content.screen->BindAndDraw();
}
