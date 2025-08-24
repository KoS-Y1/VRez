#pragma once

#include "RenderPass.h"
struct DrawContent;

class LightingPass : public RenderPass {
public:
    LightingPass() = default;

    ~LightingPass() = default;

    LightingPass(const LightingPass &)            = delete;
    LightingPass(LightingPass &&)                 = delete;
    LightingPass &operator=(const LightingPass &) = delete;
    LightingPass &operator=(LightingPass &&)      = delete;

private:
    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
};