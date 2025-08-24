#include "include/LightingPass.h"

#include <include/PbrRenderer.h>

#include <include/VulkanUtil.h>

void LightingPass::Swap(LightingPass &other) noexcept {
    std::swap(m_infoRendering, other.m_infoRendering);
}

void LightingPass::Destroy() {
}

void LightingPass::CreateRenderingInfo(const RenderingConfig &config) {
    m_infoRendering = vk_util::GetRenderingInfo(config.renderArea, &config.drawAttachments, &config.depthAttachments);
}

void LightingPass::DrawCalls(const DrawContent &content, VkPipelineLayout layout) {
    content.screen->BindAndDraw();
}

void LightingPass::PreRender() {

}

void LightingPass::PostRender() {
}