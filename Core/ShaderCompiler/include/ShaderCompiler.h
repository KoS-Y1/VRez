#pragma once

#include <string>
#include <vector>
#include <map>

#include <vulkan/vulkan.h>

#include "glslang/Public/ShaderLang.h"

class ShaderCompiler
{
public:
    ShaderCompiler() = delete;

    explicit ShaderCompiler(const std::vector<std::string> &sources);

    ~ShaderCompiler();

    ShaderCompiler(const ShaderCompiler &) = delete;

    ShaderCompiler(ShaderCompiler &&) = delete;

    ShaderCompiler &operator=(const ShaderCompiler &) = delete;

    ShaderCompiler &operator=(ShaderCompiler &&) = delete;

    [[nodiscard]] std::map<VkShaderStageFlagBits, std::vector<uint32_t>>  CompileToSpirv() const { return m_spirvs; }

    [[nodiscard]] std::vector<VkDescriptorSetLayoutCreateInfo> GetDescriptorSetLayoutInfos() const
    {
        return m_descriptorSetLayoutInfos;
    }

    [[nodiscard]] std::vector<VkPushConstantRange> GetPushConstantRanges() const { return m_pushConstantRanges; }

private:
    std::map<std::uint32_t, std::vector<VkDescriptorSetLayoutBinding> > m_bindingsPerSet;
    std::vector<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutInfos;
    std::vector<VkPushConstantRange> m_pushConstantRanges;
    std::map<VkShaderStageFlagBits, std::vector<uint32_t>> m_spirvs;

    VkShaderStageFlagBits GetShaderStage(const std::string &source);

    EShLanguage GetShaderType(VkShaderStageFlagBits shaderStage);

    VkDescriptorType GetDescriptorType(const glslang::TObjectReflection &uniform);

    void Parse(const std::string &source);

    void GenerateDescriptorSetLayoutInfos();

    uint32_t GetPushConstantSize(const glslang::TObjectReflection &uniform);
};
