#include "include/ShaderCompiler.h"

#include <glslang/Public/ShaderLang.h>

#include <SDL3/SDL_log.h>
#include <SPIRV/GlslangToSpv.h>
#include <glslang/Include/Types.h>

#include <Debug.h>
#include <include/FileSystem.h>

std::shared_ptr<ShaderIncluder> ShaderCompiler::m_includer = nullptr;

struct ShaderIncluder : glslang::TShader::Includer {
public:
    ShaderIncluder()
        : m_dir(ShaderCompiler::SHADER_HEADERS_DIR) {}

    IncludeResult *includeLocal(const char *, const char *, size_t) override { return nullptr; }

    IncludeResult *includeSystem(const char *header, const char *, size_t) { return Include(header); }

    void releaseInclude(IncludeResult *include) override { delete include; }

private:
    std::string                        m_dir;
    std::map<std::string, std::string> m_headers;

    IncludeResult *Include(const std::string &header) {
        auto pair = m_headers.find(header);
        if (pair == m_headers.end()) {
            SDL_Log("Loading shader header %s", header.c_str());
            pair = m_headers.emplace(header, file_system::Read(m_dir + header)).first;
        }

        return new IncludeResult(header, pair->second.c_str(), pair->second.length(), nullptr);
    }
};

ShaderCompiler::ShaderCompiler(const std::vector<std::string> &dirs) {
    if (!m_includer) {
        m_includer = std::make_shared<ShaderIncluder>();
    }
    for (const auto &dir: dirs) {
        Compile(dir);
    }

    GenerateReflectData();
    ExtractPushConstants();
    ExtractDescriptorSets();
}

ShaderCompiler::~ShaderCompiler() {
    m_pushConstantRanges.clear();
    m_descriptorSetLayoutInfos.clear();
    m_shaderModules.clear();
}

void ShaderCompiler::Compile(const std::string &dir) {
    TBuiltInResource DefaultTBuiltInResource{
        .maxLights                                 = 32,
        .maxClipPlanes                             = 6,
        .maxTextureUnits                           = 32,
        .maxTextureCoords                          = 32,
        .maxVertexAttribs                          = 64,
        .maxVertexUniformComponents                = 4096,
        .maxVaryingFloats                          = 64,
        .maxVertexTextureImageUnits                = 32,
        .maxCombinedTextureImageUnits              = 80,
        .maxTextureImageUnits                      = 32,
        .maxFragmentUniformComponents              = 4096,
        .maxDrawBuffers                            = 32,
        .maxVertexUniformVectors                   = 128,
        .maxVaryingVectors                         = 8,
        .maxFragmentUniformVectors                 = 16,
        .maxVertexOutputVectors                    = 16,
        .maxFragmentInputVectors                   = 15,
        .minProgramTexelOffset                     = -8,
        .maxProgramTexelOffset                     = 7,
        .maxClipDistances                          = 8,
        .maxComputeWorkGroupCountX                 = 65535,
        .maxComputeWorkGroupCountY                 = 65535,
        .maxComputeWorkGroupCountZ                 = 65535,
        .maxComputeWorkGroupSizeX                  = 1024,
        .maxComputeWorkGroupSizeY                  = 1024,
        .maxComputeWorkGroupSizeZ                  = 64,
        .maxComputeUniformComponents               = 1024,
        .maxComputeTextureImageUnits               = 16,
        .maxComputeImageUniforms                   = 8,
        .maxComputeAtomicCounters                  = 8,
        .maxComputeAtomicCounterBuffers            = 1,
        .maxVaryingComponents                      = 60,
        .maxVertexOutputComponents                 = 64,
        .maxGeometryInputComponents                = 64,
        .maxGeometryOutputComponents               = 128,
        .maxFragmentInputComponents                = 128,
        .maxImageUnits                             = 8,
        .maxCombinedImageUnitsAndFragmentOutputs   = 8,
        .maxCombinedShaderOutputResources          = 8,
        .maxImageSamples                           = 0,
        .maxVertexImageUniforms                    = 0,
        .maxTessControlImageUniforms               = 0,
        .maxTessEvaluationImageUniforms            = 0,
        .maxGeometryImageUniforms                  = 0,
        .maxFragmentImageUniforms                  = 8,
        .maxCombinedImageUniforms                  = 8,
        .maxGeometryTextureImageUnits              = 16,
        .maxGeometryOutputVertices                 = 256,
        .maxGeometryTotalOutputComponents          = 1024,
        .maxGeometryUniformComponents              = 1024,
        .maxGeometryVaryingComponents              = 64,
        .maxTessControlInputComponents             = 128,
        .maxTessControlOutputComponents            = 128,
        .maxTessControlTextureImageUnits           = 16,
        .maxTessControlUniformComponents           = 1024,
        .maxTessControlTotalOutputComponents       = 4096,
        .maxTessEvaluationInputComponents          = 128,
        .maxTessEvaluationOutputComponents         = 128,
        .maxTessEvaluationTextureImageUnits        = 16,
        .maxTessEvaluationUniformComponents        = 1024,
        .maxTessPatchComponents                    = 120,
        .maxPatchVertices                          = 32,
        .maxTessGenLevel                           = 64,
        .maxViewports                              = 16,
        .maxVertexAtomicCounters                   = 0,
        .maxTessControlAtomicCounters              = 0,
        .maxTessEvaluationAtomicCounters           = 0,
        .maxGeometryAtomicCounters                 = 0,
        .maxFragmentAtomicCounters                 = 8,
        .maxCombinedAtomicCounters                 = 8,
        .maxAtomicCounterBindings                  = 1,
        .maxVertexAtomicCounterBuffers             = 0,
        .maxTessControlAtomicCounterBuffers        = 0,
        .maxTessEvaluationAtomicCounterBuffers     = 0,
        .maxGeometryAtomicCounterBuffers           = 0,
        .maxFragmentAtomicCounterBuffers           = 1,
        .maxCombinedAtomicCounterBuffers           = 1,
        .maxAtomicCounterBufferSize                = 16384,
        .maxTransformFeedbackBuffers               = 4,
        .maxTransformFeedbackInterleavedComponents = 64,
        .maxCullDistances                          = 8,
        .maxCombinedClipAndCullDistances           = 8,
        .maxSamples                                = 4,
        .limits =
            {
                     .nonInductiveForLoops                 = 1,
                     .whileLoops                           = 1,
                     .doWhileLoops                         = 1,
                     .generalUniformIndexing               = 1,
                     .generalAttributeMatrixVectorIndexing = 1,
                     .generalVaryingIndexing               = 1,
                     .generalSamplerIndexing               = 1,
                     .generalVariableIndexing              = 1,
                     .generalConstantMatrixVectorIndexing  = 1,
                     }
    };

    std::string           file        = file_system::Read(dir);
    VkShaderStageFlagBits shaderStage = GetShaderStage(dir);
    EShLanguage           shaderType  = GetShaderType(shaderStage);
    glslang::TShader      shader(shaderType);

    const char *shaderCode = file.c_str();

    shader.setStrings(&shaderCode, 1);
    shader.setPreamble("#extension GL_GOOGLE_include_directive : require\n");
    shader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, 100);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_4);
    shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_3);
    shader.setEntryPoint("main");
    SDL_Log("Start compiling %s...", dir.c_str());
    if (!shader.parse(&DefaultTBuiltInResource, 100, false, EShMsgDefault, *m_includer)) {
        SDL_Log("GLSL Parsing Failed: %s", shader.getInfoLog());
        exit(EXIT_FAILURE);
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(EShMsgDefault)) {
        SDL_Log("Linking Failed: %s", shader.getInfoLog());
        exit(EXIT_FAILURE);
    }

    // Compile to spirv code
    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(shaderType), spirv);
    m_spirvs[shaderStage] = spirv;
}

void ShaderCompiler::GenerateReflectData() {
    for (const auto &spirvCode: m_spirvs) {
        SpvReflectShaderModule module;

        DEBUG_ASSERT(
            spvReflectCreateShaderModule(spirvCode.second.size() * sizeof(uint32_t), spirvCode.second.data(), &module) == SPV_REFLECT_RESULT_SUCCESS
        );

        m_shaderModules.push_back(std::move(module));
    }
}

void ShaderCompiler::ExtractPushConstants() {
    for (const auto &module: m_shaderModules) {
        uint32_t count = 0;

        // Get count
        DEBUG_ASSERT(spvReflectEnumeratePushConstantBlocks(&module, &count, nullptr) == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectBlockVariable *> variables(count);
        DEBUG_ASSERT(spvReflectEnumeratePushConstantBlocks(&module, &count, variables.data()) == SPV_REFLECT_RESULT_SUCCESS);

        for (const auto &variable: variables) {
            VkPushConstantRange newPushConstant{
                .stageFlags = static_cast<VkShaderStageFlags>(module.shader_stage),
                .offset     = variable->offset,
                .size       = variable->size,
            };
            bool exists = false;
            // If already exist, merge
            for (auto &pushConstant: m_pushConstantRanges) {
                if (newPushConstant.size == pushConstant.size && newPushConstant.offset == pushConstant.offset) {
                    pushConstant.stageFlags |= newPushConstant.stageFlags;
                    exists                   = true;
                    break;
                }
            }
            if (!exists) {
                m_pushConstantRanges.push_back(std::move(newPushConstant));
            }
        }
    }
}

void ShaderCompiler::ExtractDescriptorSets() {
    // Note that all the bindings of the same set must have the same stage flags
    std::map<uint32_t, VkShaderStageFlags> setStages;

    for (const auto &module: m_shaderModules) {
        uint32_t count = 0;

        //Get count
        DEBUG_ASSERT(spvReflectEnumerateDescriptorSets(&module, &count, nullptr) == SPV_REFLECT_RESULT_SUCCESS);
        std::vector<SpvReflectDescriptorSet *> descriptorSets(count);
        DEBUG_ASSERT(spvReflectEnumerateDescriptorSets(&module, &count, descriptorSets.data()) == SPV_REFLECT_RESULT_SUCCESS);

        // Get eahc set's data
        for (const auto &set: descriptorSets) {
            uint32_t setNum = set->set;

            auto pair = m_bindingsPerSet.find(setNum);

            // If the set is new
            if (pair == m_bindingsPerSet.end()) {
                m_bindingsPerSet[setNum] = std::vector<VkDescriptorSetLayoutBinding>{};
                setStages[setNum]        = 0;
            }
            setStages[setNum] |= module.shader_stage;

            // Get each binding's data
            for (size_t i = 0; i < set->binding_count; ++i) {
                VkDescriptorSetLayoutBinding binding;
                binding.binding            = set->bindings[i]->binding;
                binding.descriptorType     = static_cast<VkDescriptorType>(set->bindings[i]->descriptor_type);
                binding.descriptorCount    = 1;
                binding.pImmutableSamplers = nullptr;
                for (size_t iDim = 0; iDim < set->bindings[i]->array.dims_count; ++iDim) {
                    binding.descriptorCount *= set->bindings[i]->array.dims[iDim];
                }
                binding.stageFlags = module.shader_stage;

                // Check if the binding already exist
                bool exists = false;
                for (auto &b: m_bindingsPerSet[setNum]) {
                    if (b.binding == binding.binding) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    m_bindingsPerSet[setNum].push_back(std::move(binding));
                }
            }
        }
    }

    for (auto &bindings: m_bindingsPerSet) {
        for (auto &b: bindings.second) {
            b.stageFlags = setStages[bindings.first];
        }
        VkDescriptorSetLayoutCreateInfo infoLayout{
            .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext        = nullptr,
            .flags        = 0,
            .bindingCount = static_cast<uint32_t>(bindings.second.size()),
            .pBindings    = bindings.second.data()
        };

        m_descriptorSetLayoutInfos.push_back(std::move(infoLayout));
    }
}

VkShaderStageFlagBits ShaderCompiler::GetShaderStage(const std::string &dir) {
    VkShaderStageFlagBits shaderStage;
    if (dir.ends_with(".vert")) {
        shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
    } else if (dir.ends_with(".frag")) {
        shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    } else if (dir.ends_with(".comp")) {
        shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
    } else if (dir.ends_with(".geom")) {
        shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
    } else {
        SDL_Log("%s is not a valid shader!", dir.c_str());
        exit(EXIT_FAILURE);
    }
    return shaderStage;
}

EShLanguage ShaderCompiler::GetShaderType(VkShaderStageFlagBits shaderStage) {
    switch (shaderStage) {
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
