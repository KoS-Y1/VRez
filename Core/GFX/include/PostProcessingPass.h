#pragma once

#include "RenderPass.h"

class PostProcessingPass : public RenderPass {
public:
    PostProcessingPass()  = default;
    ~PostProcessingPass() = default;

    PostProcessingPass(const PostProcessingPass&) = delete;
    PostProcessingPass(PostProcessingPass&&) = delete;
    PostProcessingPass& operator=(const PostProcessingPass&) = delete;
    PostProcessingPass& operator=(PostProcessingPass&&) = delete;

private:
    void CreateRenderingInfo(const RenderingConfig &config, const DrawContent &content) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
};