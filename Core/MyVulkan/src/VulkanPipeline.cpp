#include "include/VulkanPipeline.h"

#include <filesystem>

#include <include/ShaderCompiler.h>
#include <include/VulkanUtil.h>

void VulkanPipeline::Destroy() {
    if (m_device != VK_NULL_HANDLE) {
        for (auto &shaderModule: m_shaderModules) {
            vkDestroyShaderModule(m_device, shaderModule.second, nullptr);
        }

        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        for (size_t i = 0; i < m_descriptorSetLayouts.size(); i++) {
            vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayouts[i], nullptr);
        }
    }

    m_descriptorSetLayouts.clear();
    m_pushConstantRanges.clear();
    m_shaderModules.clear();
    m_device   = VK_NULL_HANDLE;
    m_pipeline = VK_NULL_HANDLE;
    m_layout   = VK_NULL_HANDLE;
}

void VulkanPipeline::CreateShaderModules(const ShaderCompiler &shaderCompiler) {
    for (const auto shaderCode: shaderCompiler.CompileToSpirv()) {
        VkShaderModule           shaderModule = VK_NULL_HANDLE;
        VkShaderModuleCreateInfo infoModule{
            .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext    = nullptr,
            .flags    = 0,
            .codeSize = shaderCode.second.size() * sizeof(uint32_t),
            .pCode    = shaderCode.second.data()
        };
        DEBUG_VK_ASSERT(vkCreateShaderModule(m_device, &infoModule, nullptr, &shaderModule));

        m_shaderModules.emplace(shaderCode.first, std::move(shaderModule));
    }
}

void VulkanPipeline::CreateLayout() {
    VkPipelineLayoutCreateInfo infoLayout{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .setLayoutCount         = static_cast<uint32_t>(m_descriptorSetLayouts.size()),
        .pSetLayouts            = m_descriptorSetLayouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(m_pushConstantRanges.size()),
        .pPushConstantRanges    = m_pushConstantRanges.data()
    };

    vkCreatePipelineLayout(m_device, &infoLayout, nullptr, &m_layout);
}

void VulkanPipeline::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutCreateInfo> &infos) {
    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;

    for (size_t i = 0; i < infos.size(); i++) {
        DEBUG_VK_ASSERT(vkCreateDescriptorSetLayout(m_device, &infos[i], nullptr, &setLayout));
        m_descriptorSetLayouts.push_back(std::move(setLayout));
    }
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanPipeline::CreateShaderStages() {
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    for (auto &shaderModule: m_shaderModules) {
        VkPipelineShaderStageCreateInfo infoStage{
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = shaderModule.first,
            .module              = shaderModule.second,
            .pName               = "main",
            .pSpecializationInfo = nullptr
        };
        shaderStages.push_back(std::move(infoStage));
    }
    return shaderStages;
}
