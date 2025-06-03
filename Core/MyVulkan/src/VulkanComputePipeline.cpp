#include "include/VulkanComputePipeline.h"

#include <filesystem>

#include <Debug.h>

VulkanComputePipeline::VulkanComputePipeline(VkDevice device, const std::vector<std::string> &paths,
                               const std::vector<DescriptorSetLayoutConfig> &configs,
                               const std::vector<VkPushConstantRange> &constantRange)
{
    m_device = device;
    CreateDescriptorSetLayout(configs);
    CreateLayout(constantRange);
    CreatePipeline(paths);
}

void VulkanComputePipeline::CreatePipeline(const std::vector<std::string> &paths)
{
    if (paths.size() > 1)
    {
        SDL_Log("Warning: passing more than one shaders to computer shader! Program is ignoring the rest!");
    }

    std::string path = paths[0];
    CreateShaderModule(path);

    std::filesystem::path shaderPath(path);

    VkComputePipelineCreateInfo infoCompute
    {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = CreateShaderStage(path, 0),
        .layout = m_layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    DEBUG_VK_ASSERT(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &infoCompute, nullptr, &m_pipeline));
}
