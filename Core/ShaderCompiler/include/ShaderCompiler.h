#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <spirv_reflect.h>
#include <vulkan/vulkan.h>

#include <glslang/Public/ShaderLang.h>

struct ShaderIncluder;

class ShaderCompiler {
public:
    static constexpr const char* SHADER_HEADERS_DIR = "../Assets/Shaders/Headers/";

    ShaderCompiler() = delete;

    explicit ShaderCompiler(const std::vector<std::string> &dirs);

    ~ShaderCompiler();

    ShaderCompiler(const ShaderCompiler &) = delete;

    ShaderCompiler(ShaderCompiler &&) = delete;

    ShaderCompiler &operator=(const ShaderCompiler &) = delete;

    ShaderCompiler &operator=(ShaderCompiler &&) = delete;

    [[nodiscard]] std::map<VkShaderStageFlagBits, std::vector<uint32_t>> CompileToSpirv() const { return m_spirvs; }

    [[nodiscard]] std::vector<VkDescriptorSetLayoutCreateInfo> GetDescriptorSetLayoutInfos() const { return m_descriptorSetLayoutInfos; }

    [[nodiscard]] std::vector<VkPushConstantRange> GetPushConstantRanges() const { return m_pushConstantRanges; }

private:
    std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> m_bindingsPerSet;

    std::vector<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutInfos;
    std::vector<VkPushConstantRange>             m_pushConstantRanges;

    std::map<VkShaderStageFlagBits, std::vector<uint32_t>> m_spirvs;
    std::vector<SpvReflectShaderModule>                    m_shaderModules;

    static std::shared_ptr<ShaderIncluder> m_includer;

    void Compile(const std::string &dir);

    void GenerateReflectData();

    void ExtractPushConstants();

    void ExtractDescriptorSets();

    EShLanguage GetShaderType(VkShaderStageFlagBits shaderStage);

    VkShaderStageFlagBits GetShaderStage(const std::string &dir);
};
