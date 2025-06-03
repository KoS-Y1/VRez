#pragma once
#include <string>
#include <vector>

#include <Vulkan/vulkan.h>

#define MAX_DESCRIPTOR_SET_COUNT 16


struct PipelineOption
{
};

struct DescriptorSetLayoutConfig
{
    VkDescriptorSetLayoutCreateFlags flag;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};


class VulkanPipeline
{
public:

    VulkanPipeline() = default;
    ~VulkanPipeline() { Destroy(); };

    VulkanPipeline(const VulkanPipeline &) = delete;

    VulkanPipeline(VulkanPipeline &&other) noexcept { Swap(other); };

    VulkanPipeline &operator=(const VulkanPipeline &) = delete;

    VulkanPipeline &operator=(VulkanPipeline &&other)
    {
        if (this != &other)
        {
            Destroy();
            Swap(other);
        }

        return *this;
    };

    void Destroy();

    void Swap(VulkanPipeline &other) noexcept;

    [[nodiscard]] const std::vector<VkDescriptorSetLayout> &GetDescriptorSetLayouts() const
    {
        return descriptorSetLayouts;
    };
    [[nodiscard]] const VkPipeline &GetPipeline() const { return m_pipeline; };
    [[nodiscard]] const VkPipelineLayout &GetLayout() const { return m_layout; };

protected:
    VkPipelineLayout m_layout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    std::vector<VkShaderModule> m_shaderModules;
    VkDevice m_device = VK_NULL_HANDLE;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    virtual void CreatePipeline(const std::vector<std::string> &paths) = 0;

    void CreateLayout(const std::vector<VkPushConstantRange> &constantRange);

    void CreateShaderModule(const std::string &path);

    void CreateDescriptorSetLayout(const std::vector<DescriptorSetLayoutConfig> &configs);

    VkPipelineShaderStageCreateInfo  CreateShaderStage(const std::string &path, size_t shaderModuleIdx);
};
