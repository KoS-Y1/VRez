#include "include/VulkanComputePipeline.h"

#include <filesystem>

#include <Debug.h>

#include <include/VulkanState.h>
#include <include/ShaderCompiler.h>

void VulkanComputePipeline::CreatePipeline(const ShaderCompiler &shaderCompiler) {
    CreateShaderModules(shaderCompiler);

    VkComputePipelineCreateInfo infoCompute{
        .sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .stage              = CreateShaderStages()[0],
        .layout             = m_layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex  = -1
    };

    DEBUG_VK_ASSERT(vkCreateComputePipelines(VulkanState::GetInstance().GetDevice(), VK_NULL_HANDLE, 1, &infoCompute, nullptr, &m_pipeline));
}
