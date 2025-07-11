#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "glslang/Public/ShaderLang.h"

class ShaderCompiler
{
public:
    ShaderCompiler() = delete;

    explicit ShaderCompiler(const std::string &source);

    ~ShaderCompiler() = default;

    ShaderCompiler(const ShaderCompiler &) = delete;

    ShaderCompiler(ShaderCompiler &&) = delete;

    ShaderCompiler &operator=(const ShaderCompiler &) = delete;

    ShaderCompiler &operator=(ShaderCompiler &&) = delete;

    std::vector<uint32_t> CompileToSpirv();

    std::vector<VkDescriptorSetLayoutCreateInfo *> GetDescriptorSetLayoutInfos();

    std::vector<VkPushConstantRange> GetPushConstantRanges();

private:
    VkShaderStageFlagBits m_shaderStage;
    glslang::TProgram m_program;

    void GetShaderStage(const std::string &source);
    void CreateShaderProgram(const std::string &source);

    // Helper functions
    EShLanguage GetShaderType();
};
