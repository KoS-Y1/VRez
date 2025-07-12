#pragma once
#include "VulkanPipeline.h"

#include <string>

class VulkanComputePipeline : public VulkanPipeline
{
public:
    VulkanComputePipeline() = delete;

    VulkanComputePipeline(VkDevice device, const std::vector<std::string> &paths);

private:
    void CreatePipeline(const ShaderCompiler &shaderCompiler);
};
