#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "RenderPass.h"

class VulkanTexture;
class VulkanMesh;
struct DrawContent;

class SkyboxPass : RenderPass {
public:
    SkyboxPass() = default;

    SkyboxPass(const std::string &file, VkDescriptorSetLayout layout);

    ~SkyboxPass() { Destroy(); }

    SkyboxPass(const SkyboxPass &)            = delete;
    SkyboxPass &operator=(const SkyboxPass &) = delete;

    SkyboxPass(SkyboxPass &&other) { Swap(other); }

    SkyboxPass &operator=(SkyboxPass &&other) {
        if (this != &other) {
            Destroy();
            Swap(other);
        }

        return *this;
    }

    void Destroy();

    void Swap(SkyboxPass &other) noexcept;


private:
    const VulkanMesh    *m_mesh     = nullptr;
    const VulkanTexture *m_emissive = nullptr;

    VkDescriptorSet m_textureSet = VK_NULL_HANDLE;

    void OneTimeUpdateDescriptorSets();

    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
    void PreRender() override;
    void PostRender() override;
};