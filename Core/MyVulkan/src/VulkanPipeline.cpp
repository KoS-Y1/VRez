#include "include/VulkanPipeline.h"

#include <filesystem>

#include <include/ShaderCompiler.h>
#include <include/VulkanUtil.h>
#include <include/FileSystem.h>

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

VkPipelineShaderStageCreateInfo  VulkanPipeline::CreateShaderStage(std::string path, size_t shaderModuleIdx)
{
    VkPipelineShaderStageCreateInfo infoStage
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = vk_util::GetStage(path),
        .module = shaderModules[shaderModuleIdx],
        .pName = "main",
        .pSpecializationInfo = nullptr
    };

    return infoStage;
}