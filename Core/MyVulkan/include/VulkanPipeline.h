#pragma once
#include <string>
#include <vector>

#include <Vulkan/vulkan.h>

class VulkanPipeline
{
public:
    VulkanPipeline() = default;

    VulkanPipeline(VkDevice device, std::vector<std::string> paths);

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
    };

    void Destroy();

    void Swap(VulkanPipeline &other) noexcept;

private:
    VkPipelineLayout layout;
    VkPipeline pipeline;
    VkDevice m_device;

    void CreateLayout();

    void CreatePipeline(std::vector<std::string> paths);

    VkShaderModule CreateShaderModule(std::string path);

    void CreateGraphicsPipeline(std::vector<std::string> paths);
    void CreateComputePipeline(std::string path);
};
