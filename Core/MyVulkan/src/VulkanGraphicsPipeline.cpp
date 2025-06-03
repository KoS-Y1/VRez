#include "include/VulkanGraphicsPipeline.h"

#include "Debug.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, const std::vector<std::string> &paths,
                                               const GraphicsPipelineConfig config,
                                               const std::vector<DescriptorSetLayoutConfig> &configs,
                                               const std::vector<VkPushConstantRange> &constantRange,
                                               const std::vector<VkFormat> &colorFormats,
                                               const VkFormat depthFormat, VkFormat stencilFormat)
{
    m_device = device;
    m_config = config;
    m_colorFormats = colorFormats;
    m_depthFormat = depthFormat;
    m_stencilFormat = stencilFormat;
    CreateDescriptorSetLayout(configs);
    CreateLayout(constantRange);
    CreatePipeline(paths);
}

void VulkanGraphicsPipeline::CreatePipeline(const std::vector<std::string> &paths)
{
    // Create all shader modules first
    for (auto &path: paths)
    {
        CreateShaderModule(path);
    }

    // Create all shader stages with the created shader modules
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    for (size_t i = 0; i < paths.size(); i++)
    {
        shaderStages.push_back(CreateShaderStage(paths[i], i));
    }

    // Using dynamic rendering
    VkPipelineRenderingCreateInfo infoRendering
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(m_colorFormats.size()),
        .pColorAttachmentFormats = m_colorFormats.data(),
        .depthAttachmentFormat = m_depthFormat,
        .stencilAttachmentFormat = m_stencilFormat
    };

    // TODO: vertex input state info should be read from input
    // Right now using only empty info to see if the graphics pipelines work as expected
    VkPipelineVertexInputStateCreateInfo infoVertex
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    VkPipelineInputAssemblyStateCreateInfo infoInputAssembly
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = m_config.topology,
        .primitiveRestartEnable = VK_FALSE
    };

    // We don't support multiple viewports or scissors
    // We are using dynamic viewport and scissors
    VkPipelineViewportStateCreateInfo infoViewport
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    VkPipelineRasterizationStateCreateInfo infoRasterization
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = m_config.polygonMode,
        .cullMode = m_config.cullMode,
        .frontFace = m_config.frontFace,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    // TODO: currently using no anti-aliasing by default, needs update in the future
    VkPipelineMultisampleStateCreateInfo infoMultisample
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    VkPipelineDepthStencilStateCreateInfo infoDepthStencil
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = m_config.depthTestEnable,
        .depthWriteEnable = m_config.depthWriteEnable,
        .depthCompareOp = m_config.compareOp,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f
    };

    // TODO: color blend state should be read from the input
    // Setup a dummy color blending for now
    VkPipelineColorBlendAttachmentState colorBlendAttachment
    {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo infoColorBlend
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_CLEAR,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    std::vector<VkDynamicState> dynamicStates
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo infoDynamic
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    // We don't support tessellation
    VkGraphicsPipelineCreateInfo infoPipeline
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &infoRendering,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &infoVertex,
        .pInputAssemblyState = &infoInputAssembly,
        .pTessellationState = nullptr,
        .pViewportState = &infoViewport,
        .pRasterizationState = &infoRasterization,
        .pMultisampleState = &infoMultisample,
        .pDepthStencilState = &infoDepthStencil,
        .pColorBlendState = &infoColorBlend,
        .pDynamicState = &infoDynamic,
        .layout = m_layout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    DEBUG_VK_ASSERT(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &infoPipeline, nullptr, &m_pipeline));
}
