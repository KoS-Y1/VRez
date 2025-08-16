#include "include/PipelineManager.h"

#include <SDL3/SDL.h>

#include <include/ThreadPool.h>
#include <include/VertexFormats.h>
#include <include/VulkanComputePipeline.h>
#include <include/VulkanGraphicsPipeline.h>
#include <include/VulkanState.h>

std::unique_ptr<VulkanPipeline> PipelineManager::CreateResource(const std::string &key, const std::vector<std::string> files) {
    SDL_Log("Creating %s pipeline", key.c_str());

    return std::make_unique<VulkanComputePipeline>(files);
}

std::unique_ptr<VulkanPipeline> PipelineManager::CreateResource(
    const std::string             &key,
    const std::vector<std::string> files,
    const GraphicsPipelineOption  &option
) {
    SDL_Log("Creating %s pipeline", key.c_str());

    return std::make_unique<VulkanGraphicsPipeline>(files, option);
}

void PipelineManager::Init() {
    std::vector<std::pair<std::string, std::vector<std::string>>> gfxPipelines{
        {"basic_gfx",  {"../Assets/Shaders/BasicShader/basic.vert", "../Assets/Shaders/BasicShader/basic.frag"}},
        {"skybox_gfx", {"../Assets/Shaders/Skybox/Skybox.vert", "../Assets/Shaders/Skybox/Skybox.frag"}        }
    };
    std::vector<GraphicsPipelineOption> gfxOptions{
        {
         .infoVertex           = VertexPNTT::GetVertexInputStateCreateInfo(),
         .colorFormats         = {VK_FORMAT_R16G16B16A16_SFLOAT},
         .depthTestEnable      = VK_TRUE,
         .depthWriteEnable     = VK_TRUE,
         .depthCompareOp       = VK_COMPARE_OP_LESS_OR_EQUAL,
         .rasterizationSamples = VulkanState::GetInstance().GetSampleCount(),
         },
        {
         .topology             = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
         .cullMode             = VK_CULL_MODE_NONE,
         .infoVertex           = VertexP::GetVertexInputStateCreateInfo(),
         .colorFormats         = {VK_FORMAT_R16G16B16A16_SFLOAT},
         .rasterizationSamples = VulkanState::GetInstance().GetSampleCount(),
         }
    };

    for (size_t i = 0; i < gfxPipelines.size(); i++) {
        ThreadPool::GetInstance().Enqueue([this, i, gfxPipelines, gfxOptions]() {
            Preload(gfxPipelines[i].first, gfxPipelines[i].second, gfxOptions[i]);
        });
    }
}
