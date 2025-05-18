#include "include/VulkanPipeline.h"

#include <filesystem>

#include <include/ShaderCompiler.h>
#include <include/VulkanUtil.h>

VulkanPipeline::VulkanPipeline(VkDevice device, std::vector<std::string> paths)
{
    m_device = device;
    CreateLayout();
    CreatePipeline(paths);
}

void VulkanPipeline::Destroy()
{
    if (m_device != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device, pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, layout, nullptr);
    }

    m_device = VK_NULL_HANDLE;
    pipeline = VK_NULL_HANDLE;
    layout = VK_NULL_HANDLE;
}

void VulkanPipeline::Swap(VulkanPipeline &other) noexcept
{
    m_device = other.m_device;

    std::swap(pipeline, other.pipeline);
    std::swap(layout, other.layout);
}

VkShaderModule VulkanPipeline::CreateShaderModule(std::string path)
{
    VkShaderModule module = VK_NULL_HANDLE;

    VkShaderStageFlagBits stage = vk_util::GetStage(path);
    std::vector<uint32_t> shaderCode = shader_compiler::CompileToSpirv(path, stage);

    VkShaderModuleCreateInfo infoModule
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = shaderCode.size() * sizeof(uint32_t),
        .pCode = shaderCode.data()
    };

    DEBUG_VK_ASSERT(vkCreateShaderModule(m_device, &infoModule, nullptr, &module));

    return module;
}

void VulkanPipeline::CreateLayout()
{
    // TODO: desccription layout and other stuff, right now just for testing if compiling is working
    VkPipelineLayoutCreateInfo infoLayout
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    vkCreatePipelineLayout(m_device, &infoLayout, nullptr, &layout);
}

void VulkanPipeline::CreatePipeline(std::vector<std::string> paths)
{
    // Get the shader stage from the first passed shader code
    VkShaderStageFlagBits stage = vk_util::GetStage(paths[0]);

    switch (stage)
    {
        case VK_SHADER_STAGE_VERTEX_BIT:
        case VK_SHADER_STAGE_FRAGMENT_BIT:
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            CreateGraphicsPipeline(paths);
            break;

        case VK_SHADER_STAGE_COMPUTE_BIT:
            if (paths.size() > 1)
            {
                SDL_Log("Warning: passing more than one shaders to computer shader! Program is ignoring the rest!");
            }
            CreateComputePipeline(paths[0]);
            break;

        default:
            break;
    }
}

void VulkanPipeline::CreateComputePipeline(std::string path)
{
    VkShaderModule shaderModule = CreateShaderModule(path);

    std::filesystem::path shaderPath(path);

    VkPipelineShaderStageCreateInfo infoStage
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = shaderModule,
        .pName = shaderPath.filename().string().c_str(),
        .pSpecializationInfo = nullptr
    };

    VkComputePipelineCreateInfo infoCompute
    {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = infoStage,
        .layout = layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    DEBUG_VK_ASSERT(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &infoCompute, nullptr, &pipeline));
}

void VulkanPipeline::CreateGraphicsPipeline(std::vector<std::string> paths)
{
    // TODO
}
