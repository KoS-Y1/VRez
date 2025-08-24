#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "RenderPass.h"

class VulkanTexture;
class VulkanMesh;
struct DrawContent;

class SkyboxPass : public RenderPass {
public:
    SkyboxPass() = default;

    SkyboxPass(const std::string &file, VkDescriptorSetLayout layout);

    ~SkyboxPass();

    SkyboxPass(const SkyboxPass &)            = delete;
    SkyboxPass(SkyboxPass &&)                 = delete;
    SkyboxPass &operator=(const SkyboxPass &) = delete;
    SkyboxPass &operator=(SkyboxPass &&)      = delete;

private:
    const VulkanMesh    *m_mesh     = nullptr;
    const VulkanTexture *m_emissive = nullptr;

    VkDescriptorSet m_textureSet = VK_NULL_HANDLE;

    void OneTimeUpdateDescriptorSets();

    void CreateRenderingInfo(const RenderingConfig &config) override;
    void DrawCalls(const DrawContent &content, VkPipelineLayout layout) override;
};