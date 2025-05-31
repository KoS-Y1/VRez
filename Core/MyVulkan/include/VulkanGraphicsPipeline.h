#pragma once

#include <vulkan/vulkan.h>

#include "VulkanPipeline.h"

struct GraphicsPipelineConfig
{
    // TODO: add more options in the future if needed
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
    VkBool32 depthTestEnable =  VK_FALSE;
    VkBool32 depthWriteEnable = VK_FALSE;
    VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
};

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
    VulkanGraphicsPipeline() = delete;

    VulkanGraphicsPipeline(VkDevice device, const std::vector<std::string> &paths, const GraphicsPipelineConfig config,
                           const std::vector<DescriptorSetLayoutConfig> &configs = {},
                           const std::vector<VkPushConstantRange> &constantRange = {},
                           const std::vector<VkFormat> &colorFormats = {},
                           const VkFormat depthFormat = VK_FORMAT_UNDEFINED,
                           const VkFormat stencilFormat = VK_FORMAT_UNDEFINED);

private:
    GraphicsPipelineConfig m_config;
    std::vector<VkFormat> m_colorFormats;
    VkFormat m_depthFormat;
    VkFormat m_stencilFormat;

    void CreatePipeline(const std::vector<std::string> &paths);

};
