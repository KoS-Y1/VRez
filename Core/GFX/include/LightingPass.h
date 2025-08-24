#pragma once

#include "RenderPass.h"
struct DrawContent;

class LightingPass : public RenderPass {
public:
    LightingPass() = default;

    ~LightingPass() = default;

    LightingPass(const LightingPass &)            = delete;
    LightingPass &operator=(const LightingPass &) = delete;

    LightingPass(LightingPass &&other) { Swap(other); }

    LightingPass &operator=(LightingPass &&other) {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void Swap(LightingPass &other) noexcept;
    void Destroy();

private:
    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
    void PreRender() override;
    void PostRender() override;
};