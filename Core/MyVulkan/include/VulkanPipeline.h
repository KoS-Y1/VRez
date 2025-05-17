#pragma once
#include <string>
#include <vector>

#include <Vulkan/vulkan.h>

class VulkanPipeline
{
public:
    VulkanPipeline() = delete;
    VulkanPipeline(std::vector<std::string> paths);
    ~VulkanPipeline() { Destroy(); };

    // Disallow copy and move
    VulkanPipeline(const VulkanPipeline &) = delete;

    VulkanPipeline(VulkanPipeline &&) = delete;

    VulkanPipeline &operator=(const VulkanPipeline &) = delete;

    VulkanPipeline &operator=(VulkanPipeline &&) = delete;

    void Destroy();

private:
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};
