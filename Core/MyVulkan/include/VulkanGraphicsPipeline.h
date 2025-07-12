#pragma once
#include "VulkanPipeline.h"

#include <vulkan/vulkan.h>

#include <string>

struct GraphicsPipelineConfig
{
    // TODO: add more options in the future if needed
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
    VkBool32 depthTestEnable =  VK_FALSE;
    VkBool32 depthWriteEnable = VK_FALSE;
    VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
    const VkPipelineVertexInputStateCreateInfo *infoVertex;

    std::vector<VkFormat> colorFormats {};
    VkFormat depthFormat;
    VkFormat stencilFormat;
};

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
    VulkanGraphicsPipeline() = delete;

    VulkanGraphicsPipeline(VkDevice device, const std::vector<std::string> &paths, const GraphicsPipelineConfig &config);

private:
    GraphicsPipelineConfig m_config;

    void CreatePipeline(const ShaderCompiler &shaderCompiler);
};
