#include "include/VulkanPipeline.h"

#include <filesystem>

#include <include/ShaderCompiler.h>
#include <include/VulkanUtil.h>
#include <include/FileSystem.h>

void VulkanPipeline::Destroy()
{
    if (m_device != VK_NULL_HANDLE)
    {
        for (size_t i = 0; i < m_shaderModules.size(); i++)
        {
            vkDestroyShaderModule(m_device, m_shaderModules[i], nullptr);
        }

        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        for (size_t i = 0; i < m_descriptorSetLayouts.size(); i++)
        {
            vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayouts[i], nullptr);
        }
    }

    m_descriptorSetLayouts.clear();
    m_pushConstantRanges.clear();
    m_shaderModules.clear();
    m_device = VK_NULL_HANDLE;
    m_pipeline = VK_NULL_HANDLE;
    m_layout = VK_NULL_HANDLE;
}

// Create shader module and generate config from shader compiler
void VulkanPipeline::CreateShaderModule(const std::string &path)
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

    // Add descriptor set layout info to vector(append)
    // Add push constant info
    // Handle merging same descriptor sets/push constants using in multiple shader stages
    m_shaderModules.push_back(std::move(shaderModule));
}

// void VulkanPipeline::CreateLayout(const std::vector<VkPushConstantRange> &constantRange)
// {
//     // TODO: desccription layout and other stuff, right now just for testing if compiling is working
//     VkPipelineLayoutCreateInfo infoLayout
//     {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
//         .pNext = nullptr,
//         .setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size()),
//         .pSetLayouts = m_descriptorSetLayouts.data(),
//         .pushConstantRangeCount = static_cast<uint32_t>(constantRange.size()),
//         .pPushConstantRanges = constantRange.data()
//     };
//
//     vkCreatePipelineLayout(m_device, &infoLayout, nullptr, &m_layout);
// }

void VulkanPipeline::CreateLayout()
{
    VkPipelineLayoutCreateInfo infoLayout
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size()),
        .pSetLayouts = m_descriptorSetLayouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(m_pushConstantRanges.size()),
        .pPushConstantRanges = m_pushConstantRanges.data()
    };

    vkCreatePipelineLayout(m_device, &infoLayout, nullptr, &m_layout);
}

// void VulkanPipeline::CreateDescriptorSetLayout(const std::vector<DescriptorSetLayoutConfig> &configs)
// {
//     VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
//
//     for (size_t i = 0; i < configs.size(); i++)
//     {
//         VkDescriptorSetLayoutCreateInfo infoLayout
//         {
//             .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
//             .flags = configs[i].flag,
//             .bindingCount = static_cast<uint32_t>(configs[i].bindings.size()),
//             .pBindings = configs[i].bindings.data(),
//         };
//         DEBUG_VK_ASSERT(vkCreateDescriptorSetLayout(m_device, &infoLayout, nullptr, &setLayout));
//         m_descriptorSetLayouts.push_back(std::move(setLayout));
//     }
// }

void VulkanPipeline::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutCreateInfo *> infos)
{
    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;

    for (size_t i = 0; i < infos.size(); i++)
    {
        DEBUG_VK_ASSERT(vkCreateDescriptorSetLayout(m_device, infos[i], nullptr, &setLayout));
        m_descriptorSetLayouts.push_back(std::move(setLayout));
    }
}


VkPipelineShaderStageCreateInfo VulkanPipeline::CreateShaderStage(const std::string &path, size_t shaderModuleIdx)
{
    VkPipelineShaderStageCreateInfo infoStage
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = vk_util::GetStage(path),
        .module = m_shaderModules[shaderModuleIdx],
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    return infoStage;
}
