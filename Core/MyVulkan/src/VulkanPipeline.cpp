#include "include/VulkanPipeline.h"

#include <filesystem>

#include <include/ShaderCompiler.h>
#include <include/VulkanUtil.h>
#include <include/FileSystem.h>

// VulkanPipeline::VulkanPipeline(VkDevice device, const std::vector<std::string> &paths,
//                                const std::vector<DescriptorSetLayoutConfig> &configs,
//                                const std::vector<VkPushConstantRange> &constantRange)
// {
//     m_device = device;
//     CreateDescriptorSetLayout(configs);
//     CreateLayout(constantRange);
//     CreatePipeline(paths);
// }

void VulkanPipeline::Destroy()
{
    if (m_device != VK_NULL_HANDLE)
    {
        for (size_t i = 0; i < shaderModules.size(); i++)
        {
            vkDestroyShaderModule(m_device, shaderModules[i], nullptr);
        }

        vkDestroyPipeline(m_device, pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, layout, nullptr);
        for (size_t i = 0; i < descriptorSetLayouts.size(); i++)
        {
            vkDestroyDescriptorSetLayout(m_device, descriptorSetLayouts[i], nullptr);
        }
    }

    descriptorSetLayouts.clear();
    shaderModules.clear();
    m_device = VK_NULL_HANDLE;
    pipeline = VK_NULL_HANDLE;
    layout = VK_NULL_HANDLE;
}

void VulkanPipeline::Swap(VulkanPipeline &other) noexcept
{
    m_device = other.m_device;

    std::swap(pipeline, other.pipeline);
    std::swap(layout, other.layout);
    std::swap(shaderModules, other.shaderModules);
    std::swap(descriptorSetLayouts, other.descriptorSetLayouts);
}

void VulkanPipeline::CreateShaderModule(const std::string path)
{
    VkShaderModule shaderModule = VK_NULL_HANDLE;

    VkShaderStageFlagBits stage = vk_util::GetStage(path);
    std::string file = file_system::Read(path);
    std::vector<uint32_t> shaderCode = shader_compiler::CompileToSpirv(file, stage);

    VkShaderModuleCreateInfo infoModule
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = shaderCode.size() * sizeof(uint32_t),
        .pCode = shaderCode.data()
    };

    DEBUG_VK_ASSERT(vkCreateShaderModule(m_device, &infoModule, nullptr, &shaderModule));

    shaderModules.push_back(std::move(shaderModule));
}

void VulkanPipeline::CreateLayout(const std::vector<VkPushConstantRange> &constantRange)
{
    // TODO: desccription layout and other stuff, right now just for testing if compiling is working
    VkPipelineLayoutCreateInfo infoLayout
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        .pSetLayouts = descriptorSetLayouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(constantRange.size()),
        .pPushConstantRanges = constantRange.data()
    };

    vkCreatePipelineLayout(m_device, &infoLayout, nullptr, &layout);
}

// void VulkanPipeline::CreatePipeline(const std::vector<std::string> &paths)
// {
//     // Get the shader stage from the first passed shader code
//     VkShaderStageFlagBits stage = vk_util::GetStage(paths[0]);
//
//     switch (stage)
//     {
//         case VK_SHADER_STAGE_VERTEX_BIT:
//         case VK_SHADER_STAGE_FRAGMENT_BIT:
//         case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
//         case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
//         case VK_SHADER_STAGE_GEOMETRY_BIT:
//             CreateGraphicsPipeline(paths);
//             break;
//
//         case VK_SHADER_STAGE_COMPUTE_BIT:
//             if (paths.size() > 1)
//             {
//                 SDL_Log("Warning: passing more than one shaders to computer shader! Program is ignoring the rest!");
//             }
//             CreateComputePipeline(paths[0]);
//             break;
//
//         default:
//             break;
//     }
// }
//
// void VulkanPipeline::CreateComputePipeline(std::string path)
// {
//     CreateShaderModule(path);
//
//     std::filesystem::path shaderPath(path);
//
//     VkPipelineShaderStageCreateInfo infoStage
//     {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .stage = VK_SHADER_STAGE_COMPUTE_BIT,
//         .module = shaderModules[0],
//         .pName = "main",
//         .pSpecializationInfo = nullptr
//     };
//
//     VkComputePipelineCreateInfo infoCompute
//     {
//         .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
//         .pNext = nullptr,
//         .flags = 0,
//         .stage = infoStage,
//         .layout = layout,
//         .basePipelineHandle = VK_NULL_HANDLE,
//         .basePipelineIndex = -1
//     };
//
//     DEBUG_VK_ASSERT(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &infoCompute, nullptr, &pipeline));
// }
//
// void VulkanPipeline::CreateGraphicsPipeline(std::vector<std::string> paths)
// {
//     // TODO
// }

void VulkanPipeline::CreateDescriptorSetLayout(const std::vector<DescriptorSetLayoutConfig> &configs)
{
    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;

    for (size_t i = 0; i < configs.size(); i++)
    {
        VkDescriptorSetLayoutCreateInfo infoLayout
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags = configs[i].flag,
            .bindingCount = static_cast<uint32_t>(configs[i].bindings.size()),
            .pBindings = configs[i].bindings.data(),
        };
        DEBUG_VK_ASSERT(vkCreateDescriptorSetLayout(m_device, &infoLayout, nullptr, &setLayout));
        descriptorSetLayouts.push_back(std::move(setLayout));
    }
}
