#include "include/ShaderCompiler.h"

#include <glslang/Public/ShaderLang.h>

#include "glslang/Include/Types.h"
#include "SDL3/SDL_log.h"
#include "SPIRV/GlslangToSpv.h"

ShaderCompiler::ShaderCompiler(const std::string &source)
{
    GetShaderStage(source);
    CreateShaderProgram(source);

    // TODO
}

void ShaderCompiler::GetShaderStage(const std::string &source)
{
    if (source.ends_with(".vert"))
    {
        m_shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
    }
    else if (source.ends_with(".frag"))
    {
        m_shaderStage =  VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    else if (source.ends_with(".comp"))
    {
        m_shaderStage =  VK_SHADER_STAGE_COMPUTE_BIT;
    }
    else
    {
        SDL_Log("%s is not a valid shader!", source.c_str());
        exit(EXIT_FAILURE);
    }
}

void ShaderCompiler::CreateShaderProgram(const std::string &source)
{
TBuiltInResource DefaultTBuiltInResource
    {
        .maxLights = 32,
        .maxClipPlanes = 6,
        .maxTextureUnits = 32,
        .maxTextureCoords = 32,
        .maxVertexAttribs = 64,
        .maxVertexUniformComponents = 4096,
        .maxVaryingFloats = 64,
        .maxVertexTextureImageUnits = 32,
        .maxCombinedTextureImageUnits = 80,
        .maxTextureImageUnits = 32,
        .maxFragmentUniformComponents = 4096,
        .maxDrawBuffers = 32,
        .maxVertexUniformVectors = 128,
        .maxVaryingVectors = 8,
        .maxFragmentUniformVectors = 16,
        .maxVertexOutputVectors = 16,
        .maxFragmentInputVectors = 15,
        .minProgramTexelOffset = -8,
        .maxProgramTexelOffset = 7,
        .maxClipDistances = 8,
        .maxComputeWorkGroupCountX = 65535,
        .maxComputeWorkGroupCountY = 65535,
        .maxComputeWorkGroupCountZ = 65535,
        .maxComputeWorkGroupSizeX = 1024,
        .maxComputeWorkGroupSizeY = 1024,
        .maxComputeWorkGroupSizeZ = 64,
        .maxComputeUniformComponents = 1024,
        .maxComputeTextureImageUnits = 16,
        .maxComputeImageUniforms = 8,
        .maxComputeAtomicCounters = 8,
        .maxComputeAtomicCounterBuffers = 1,
        .maxVaryingComponents = 60,
        .maxVertexOutputComponents = 64,
        .maxGeometryInputComponents = 64,
        .maxGeometryOutputComponents = 128,
        .maxFragmentInputComponents = 128,
        .maxImageUnits = 8,
        .maxCombinedImageUnitsAndFragmentOutputs = 8,
        .maxCombinedShaderOutputResources = 8,
        .maxImageSamples = 0,
        .maxVertexImageUniforms = 0,
        .maxTessControlImageUniforms = 0,
        .maxTessEvaluationImageUniforms = 0,
        .maxGeometryImageUniforms = 0,
        .maxFragmentImageUniforms = 8,
        .maxCombinedImageUniforms = 8,
        .maxGeometryTextureImageUnits = 16,
        .maxGeometryOutputVertices = 256,
        .maxGeometryTotalOutputComponents = 1024,
        .maxGeometryUniformComponents = 1024,
        .maxGeometryVaryingComponents = 64,
        .maxTessControlInputComponents = 128,
        .maxTessControlOutputComponents = 128,
        .maxTessControlTextureImageUnits = 16,
        .maxTessControlUniformComponents = 1024,
        .maxTessControlTotalOutputComponents = 4096,
        .maxTessEvaluationInputComponents = 128,
        .maxTessEvaluationOutputComponents = 128,
        .maxTessEvaluationTextureImageUnits = 16,
        .maxTessEvaluationUniformComponents = 1024,
        .maxTessPatchComponents = 120,
        .maxPatchVertices = 32,
        .maxTessGenLevel = 64,
        .maxViewports = 16,
        .maxVertexAtomicCounters = 0,
        .maxTessControlAtomicCounters = 0,
        .maxTessEvaluationAtomicCounters = 0,
        .maxGeometryAtomicCounters = 0,
        .maxFragmentAtomicCounters = 8,
        .maxCombinedAtomicCounters = 8,
        .maxAtomicCounterBindings = 1,
        .maxVertexAtomicCounterBuffers = 0,
        .maxTessControlAtomicCounterBuffers = 0,
        .maxTessEvaluationAtomicCounterBuffers = 0,
        .maxGeometryAtomicCounterBuffers = 0,
        .maxFragmentAtomicCounterBuffers = 1,
        .maxCombinedAtomicCounterBuffers = 1,
        .maxAtomicCounterBufferSize = 16384,
        .maxTransformFeedbackBuffers = 4,
        .maxTransformFeedbackInterleavedComponents = 64,
        .maxCullDistances = 8,
        .maxCombinedClipAndCullDistances = 8,
        .maxSamples = 4,
        .limits =
        {
            .nonInductiveForLoops = 1,
            .whileLoops = 1,
            .doWhileLoops = 1,
            .generalUniformIndexing = 1,
            .generalAttributeMatrixVectorIndexing = 1,
            .generalVaryingIndexing = 1,
            .generalSamplerIndexing = 1,
            .generalVariableIndexing = 1,
            .generalConstantMatrixVectorIndexing = 1,
        }
    };

    EShLanguage shaderType = GetShaderType();
    glslang::TShader shader(shaderType);

    const char *shaderCode = source.c_str();

    shader.setStrings(&shaderCode, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_4);
    shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_3);
    shader.setEntryPoint("main");
    if (!shader.parse(&DefaultTBuiltInResource, 100, false, EShMsgDefault))
    {
        SDL_Log("GLSL Parsing Failed: %s", shader.getInfoLog());
        exit(EXIT_FAILURE);
    }

    m_program.addShader(&shader);

    if (!m_program.link(EShMsgDefault))
    {
        SDL_Log("Linking Failed: %s", shader.getInfoLog());
        exit(EXIT_FAILURE);
    }
}

EShLanguage ShaderCompiler::GetShaderType()
{
    switch (m_shaderStage)
    {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;

        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;

        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;

        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;

        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;

        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;

        case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
            return EShLangRayGen;

        case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
            return EShLangAnyHit;

        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            return EShLangClosestHit;

        case VK_SHADER_STAGE_MISS_BIT_KHR:
            return EShLangMiss;

        case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
            return EShLangIntersect;

        case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
            return EShLangCallable;

        case VK_SHADER_STAGE_MESH_BIT_EXT:
            return EShLangMesh;

        case VK_SHADER_STAGE_TASK_BIT_EXT:
            return EShLangTask;

        default:
            return EShLangVertex;
    }
}

std::vector<uint32_t> ShaderCompiler::CompileToSpirv()
{
    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*m_program.getIntermediate(GetShaderType()), spirv);

    return spirv;
}

// TODO: Currently the function only gets info of uniforms, need to get info of storages
// But is that really needed? So far the project is only planned to use small size data in the shader
std::vector<VkDescriptorSetLayoutCreateInfo *> ShaderCompiler::GetDescriptorSetLayoutInfos()
{
    std::vector<VkDescriptorSetLayoutCreateInfo *> descriptorSetLayoutInfos;

    for (size_t i = 0; i < m_program.getNumUniformVariables(); ++i)
    {
        const glslang::TObjectReflection& uniform = m_program.getUniform(i);

        if (!uniform.getType()->getQualifier().isPushConstant())
        {
            VkDescriptorSetLayoutCreateInfo infoLayout
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .bindingCount = // TODO: binding count is based on the bindings 
            };
        }
    }
}

std::vector<VkPushConstantRange> ShaderCompiler::GetPushConstantRanges()
{
    std::vector<VkPushConstantRange> pushConstantRanges;

    for (size_t i = 0; i < m_program.getNumUniformVariables(); ++i)
    {
        const glslang::TObjectReflection& uniform = m_program.getUniform(i);

        if (uniform.getType()->getQualifier().isPushConstant())
        {
            VkPushConstantRange pushConstantRange
            {
                .stageFlags = static_cast<VkShaderStageFlags>(m_shaderStage),
                .offset = static_cast<uint32_t>(uniform.offset),
                .size = static_cast<uint32_t> (uniform.size)

            };
            pushConstantRanges.push_back(pushConstantRange);
        }
    }
    return pushConstantRanges;
}