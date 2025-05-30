#pragma once
#include "VulkanPipeline.h"

class VulkanComputePipeline : public VulkanPipeline
{
public:
    VulkanComputePipeline() = delete;

    VulkanComputePipeline(VkDevice device, const std::vector<std::string> &paths,
                          const std::vector<DescriptorSetLayoutConfig> &configs = {},
                          const std::vector<VkPushConstantRange> &constantRange = {});

private:
    void CreatePipeline(const std::vector<std::string> &paths);
};
