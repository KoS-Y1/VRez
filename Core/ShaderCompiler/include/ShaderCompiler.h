#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "glslang/Public/ShaderLang.h"

namespace shader_compiler
{
    void Initialize();

    void Finalize();

    std::vector<uint32_t> CompileToSpirv(const std::string &source, VkShaderStageFlagBits shaderStage);
}
