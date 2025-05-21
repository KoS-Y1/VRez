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

    VulkanPipeline(VkDevice device, std::vector<std::string> paths, std::vector<DescriptorSetLayoutConfig> configs);

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

    [[nodiscard]] const std::vector<VkDescriptorSetLayout> &GetDescriptorSetLayouts() const  { return descriptorSetLayouts; };
    [[nodiscard]] const VkPipeline &GetPipeline() const { return pipeline; };
    [[nodiscard]] const VkPipelineLayout &GetLayout() const { return layout; };

private:
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    std::vector<VkShaderModule> shaderModules;
    VkDevice m_device = VK_NULL_HANDLE;

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    void CreatePipeline(std::vector<std::string> paths);

    void CreateLayout();

    void CreateShaderModule(std::string path);

    void CreateGraphicsPipeline(std::vector<std::string> paths);

    void CreateComputePipeline(std::string path);

    void CreateDescriptorSetLayout(const std::vector<DescriptorSetLayoutConfig> &configs);
};
