#pragma once

#include "RenderPass.h"

class ForwardPass : public RenderPass {
public:
    ForwardPass()  = default;
    ~ForwardPass() = default;

    ForwardPass(const ForwardPass &)            = delete;
    ForwardPass(ForwardPass &&)                 = delete;
    ForwardPass &operator=(const ForwardPass &) = delete;
    ForwardPass &operator=(ForwardPass &&)      = delete;

private:
    void CreateRenderingInfo(const RenderingConfig &config, const DrawContent &content) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
};