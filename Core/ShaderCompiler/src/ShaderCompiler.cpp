#include "include/ShaderCompiler.h"

#include <glslang/Public/ShaderLang.h>

#include <glslang/Include/Types.h>
#include <SDL3/SDL_log.h>
#include <SPIRV/GlslangToSpv.h>

#include <include/FileSystem.h>

#include "glslang/MachineIndependent/localintermediate.h"

ShaderCompiler::ShaderCompiler(const std::vector<std::string> &sources)
{
    for (const auto &source: sources)
    {
        Parse(source);
    }

    GenerateDescriptorSetLayoutInfos();
}

ShaderCompiler::~ShaderCompiler()
{
    m_bindingsPerSet.clear();
    m_pushConstantRanges.clear();
    m_descriptorSetLayoutInfos.clear();
}

void ShaderCompiler::Parse(const std::string &source)
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

    std::string file = file_system::Read(source);
    VkShaderStageFlagBits shaderStage = GetShaderStage(source);
    EShLanguage shaderType = GetShaderType(shaderStage);
    glslang::TShader shader(shaderType);

    const char *shaderCode = file.c_str();

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

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault))
    {
        SDL_Log("Linking Failed: %s", shader.getInfoLog());
        exit(EXIT_FAILURE);
    }

    // Compile to spirv code
    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(shaderType), spirv);
    m_spirvs[shaderStage] = spirv;

    program.buildReflection();

    // Get push constant/descripot set layouts
    for (size_t i = 0; i < program.getNumUniformVariables(); ++i)
    {
        const glslang::TObjectReflection &uniform = program.getUniform(i);
        // A stupid way to determine if it is a push constant
        // We require push constant in shader must be named with prefix PC_
        if (uniform.name.find("PC_") != std::string::npos)
        {
            VkPushConstantRange pushConstantRange
            {
                .stageFlags = static_cast<VkShaderStageFlags>(shaderStage),
                .offset = static_cast<uint32_t>(uniform.offset),
                .size = GetPushConstantSize(uniform)

            };

            // Merge if it's already in the push constant array
            bool isExist = false;
            for (auto &pushConstant: m_pushConstantRanges)
            {
                if (pushConstant.stageFlags == pushConstantRange.stageFlags)
                {
                    pushConstant.size += pushConstantRange.size;
                    pushConstant.offset = std::min(pushConstant.offset, pushConstantRange.offset);
                    isExist = true;
                    break;
                }
            }
            if (!isExist)
            {
                m_pushConstantRanges.push_back(pushConstantRange);
            }
        }
        // If descriptor set
        else
        {
            // TODO: Currently only get type of image, sampler, texture, buffer, need to update for other types
            // But is that really needed? So far the project is only planned to use small size data in the shader
            uint32_t set = uniform.getType()->getQualifier().layoutSet;

            VkDescriptorSetLayoutBinding binding
            {
                .binding = uniform.getType()->getQualifier().layoutBinding,
                .descriptorType = GetDescriptorType(uniform),
                .descriptorCount = 1, // We won't use an array of descriptors
                .stageFlags = static_cast<VkShaderStageFlags>(shaderStage),
                .pImmutableSamplers = nullptr, // We won't use immutable sampler
            };

            auto pair = m_bindingsPerSet.find(set);

            // If it's a new set
            if (pair == m_bindingsPerSet.end())
            {
                m_bindingsPerSet.emplace(set, std::vector<VkDescriptorSetLayoutBinding>{});
                m_bindingsPerSet[set].push_back(binding);
            }
            // If set already exits
            else
            {
                bool isExist = false;
                for (auto &b: m_bindingsPerSet[set])
                {
                    if (binding.binding == b.binding)
                    {
                        b.stageFlags |= binding.stageFlags;
                        isExist = true;
                        break;
                    }
                }
                if (!isExist)
                {
                    m_bindingsPerSet[set].push_back(binding);
                }
            }
        }
    }
}

VkShaderStageFlagBits ShaderCompiler::GetShaderStage(const std::string &source)
{
    VkShaderStageFlagBits shaderStage;
    if (source.ends_with(".vert"))
    {
        shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
    }
    else if (source.ends_with(".frag"))
    {
        shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    else if (source.ends_with(".comp"))
    {
        shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
    }
    else
    {
        SDL_Log("%s is not a valid shader!", source.c_str());
        exit(EXIT_FAILURE);
    }
    return shaderStage;
}

EShLanguage ShaderCompiler::GetShaderType(VkShaderStageFlagBits shaderStage)
{
    switch (shaderStage)
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

VkDescriptorType ShaderCompiler::GetDescriptorType(const glslang::TObjectReflection &uniform)
{
    VkDescriptorType descriptorType;
    auto type = uniform.getType();
    switch (type->getBasicType())
    {
        case glslang::EbtSampler:

            if (type->isImage())
            {
                if (type->getSampler().isPureSampler())
                {
                    descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                }
                else if (type->getSampler().isTexture())
                {
                    descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                }
                else if (type->getSampler().isCombined())
                {
                    descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                }
                else if (type->getSampler().isImage())
                {
                    descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                }
            }
            else
            {
                descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            }
            break;
        case glslang::EbtBlock:
            if (type->getQualifier().storage == glslang::EvqUniform)
            {
                descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
            else if (type->getQualifier().storage == glslang::EvqBuffer)
            {
                descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            }
            break;
        default:
            SDL_Log("Not a valid descriptor type!");
            exit(EXIT_FAILURE);
    }

    return descriptorType;
}

void ShaderCompiler::GenerateDescriptorSetLayoutInfos()
{
    for (const auto &pair: m_bindingsPerSet)
    {
        VkDescriptorSetLayoutCreateInfo infoLayout
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<uint32_t>(pair.second.size()),
            .pBindings = pair.second.data(),
        };
        m_descriptorSetLayoutInfos.push_back(infoLayout);
    }
}

uint32_t ShaderCompiler::GetPushConstantSize(const glslang::TObjectReflection &uniform)
{
    uint32_t totalSize = 0;

    size_t arraySize = uniform.size;
    auto type = uniform.getType();
    auto typeSize = [](glslang::TBasicType type)
    {
        uint32_t size = 0;
        switch (type)
        {
            case glslang::EbtFloat:
                size = sizeof(float);
                break;
            case glslang::EbtDouble:
                size = sizeof(double);
                break;
            case glslang::EbtInt8:
                size = sizeof(int8_t);
                break;
            case glslang::EbtUint8:
                size = sizeof(uint8_t);
                break;
            case glslang::EbtInt16:
                size = sizeof(int16_t);
                break;
            case glslang::EbtUint16:
                size = sizeof(uint16_t);
                break;
            // TODO: other type, but we probaly won't use those in our project
        }
        return size;
    }(type->getBasicType());

    if (type->getVectorSize() > 0)
    {
        totalSize = arraySize * typeSize * type->getVectorSize();
    }
    if (type->getMatrixCols() > 0)
    {
        totalSize = arraySize * typeSize * type->getMatrixCols() * type->getMatrixRows();
    }

    return totalSize;
}
