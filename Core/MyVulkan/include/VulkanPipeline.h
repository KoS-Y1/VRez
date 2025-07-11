#pragma once
#include <string>
#include <vector>

#include <Vulkan/vulkan.h>

#define MAX_DESCRIPTOR_SET_COUNT 16

// struct DescriptorSetLayoutConfig
// {
//     VkDescriptorSetLayoutCreateFlags flag;
//     std::vector<VkDescriptorSetLayoutBinding> bindings;
// };


class VulkanPipeline
{
public:
    VulkanPipeline() = default;

    virtual ~VulkanPipeline() { Destroy(); };

    VulkanPipeline(const VulkanPipeline &) = delete;

    VulkanPipeline(VulkanPipeline &&) = delete;

    VulkanPipeline &operator=(const VulkanPipeline &) = delete;

    VulkanPipeline &operator=(VulkanPipeline &&) = delete;

    void Destroy();


    [[nodiscard]] const std::vector<VkDescriptorSetLayout> &GetDescriptorSetLayouts() const
    {
        return m_descriptorSetLayouts;
    };
    [[nodiscard]] const VkPipeline &GetPipeline() const { return m_pipeline; };
    [[nodiscard]] const VkPipelineLayout &GetLayout() const { return m_layout; };

protected:
    VkPipelineLayout m_layout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    std::vector<VkShaderModule> m_shaderModules;
    VkDevice m_device = VK_NULL_HANDLE;

    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;
    std::vector<VkPushConstantRange> m_pushConstantRanges;
    // PipelineLayoutConfig m_pipelineLayoutConfig;

    // void CreateLayout(const std::vector<VkPushConstantRange> &constantRange);
    void CreateLayout();

    void CreateShaderModule(const std::string &path);

    // void CreateDescriptorSetLayout(const std::vector<DescriptorSetLayoutConfig> &configs);
    void CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutCreateInfo*> infos);
    VkPipelineShaderStageCreateInfo CreateShaderStage(const std::string &path, size_t shaderModuleIdx);
};
