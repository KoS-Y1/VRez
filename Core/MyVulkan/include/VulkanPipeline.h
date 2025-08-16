#pragma once

#include <map>
#include <vector>

#include <include/ShaderCompiler.h>

#define MAX_DESCRIPTOR_SET_COUNT 16

class VulkanPipeline {
public:
    VulkanPipeline() = default;

    virtual ~VulkanPipeline() { Destroy(); };


    VulkanPipeline(const std::vector<std::string> &paths);

    VulkanPipeline(const VulkanPipeline &)            = delete;
    VulkanPipeline(VulkanPipeline &&)                 = delete;
    VulkanPipeline &operator=(const VulkanPipeline &) = delete;
    VulkanPipeline &operator=(VulkanPipeline &&)      = delete;

    void Destroy();
    [[nodiscard]] const std::vector<VkDescriptorSetLayout> &GetDescriptorSetLayouts() const { return m_descriptorSetLayouts; };

    [[nodiscard]] const VkPipeline &GetPipeline() const { return m_pipeline; };

    [[nodiscard]] const VkPipelineLayout &GetLayout() const { return m_layout; };

protected:
    VkPipelineLayout                                m_layout   = VK_NULL_HANDLE;
    VkPipeline                                      m_pipeline = VK_NULL_HANDLE;
    std::map<VkShaderStageFlagBits, VkShaderModule> m_shaderModules;

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<VkPushConstantRange>   m_pushConstantRanges;

    void CreateLayout();
    void CreateShaderModules(const ShaderCompiler &shaderCompiler);
    void CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutCreateInfo> &infos);
    std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStages();
};
