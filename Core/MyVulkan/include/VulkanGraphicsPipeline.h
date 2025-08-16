#pragma once
#include "VulkanPipeline.h"

#include <vulkan/vulkan.h>

#include <string>

struct GraphicsPipelineOption {
    // TODO: add more options in the future if needed
    VkPrimitiveTopology topology    = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode       polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags     cullMode    = VK_CULL_MODE_BACK_BIT;
    VkFrontFace         frontFace   = VK_FRONT_FACE_CLOCKWISE;

    // Input vertex
    const VkPipelineVertexInputStateCreateInfo *infoVertex;

    std::vector<VkFormat> colorFormats{};

    // Depth stencil
    VkBool32    depthTestEnable  = VK_FALSE;
    VkBool32    depthWriteEnable = VK_FALSE;
    VkCompareOp depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL;
    VkFormat    depthFormat      = VK_FORMAT_D32_SFLOAT;
    VkFormat    stencilFormat    = VK_FORMAT_UNDEFINED;

    // MSAA
    VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
};

class VulkanGraphicsPipeline : public VulkanPipeline {
public:
    VulkanGraphicsPipeline() = default;

    VulkanGraphicsPipeline(const std::vector<std::string> &paths, const GraphicsPipelineOption &option)
        : VulkanPipeline(paths) {
        CreatePipeline(option);
    }

protected:
    void CreatePipeline(const GraphicsPipelineOption &option);

private:
    friend class VulkanPipeline;
};
