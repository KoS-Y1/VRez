#include "include/VulkanGraphicsPipeline.h"

#include <Debug.h>

#include <include/VulkanState.h>


void VulkanGraphicsPipeline::CreatePipeline(const GraphicsPipelineOption &option) {

    // Create all shader stages with the created shader modules
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = CreateShaderStages();

    // Using dynamic rendering
    VkPipelineRenderingCreateInfo infoRendering{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0,
        .colorAttachmentCount    = static_cast<uint32_t>(option.colorFormats.size()),
        .pColorAttachmentFormats = option.colorFormats.data(),
        .depthAttachmentFormat   = option.depthFormat,
        .stencilAttachmentFormat = option.stencilFormat,
    };


    VkPipelineInputAssemblyStateCreateInfo infoInputAssembly{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = option.topology,
        .primitiveRestartEnable = VK_FALSE
    };

    // We don't support multiple viewports or scissors
    // We are using dynamic viewport and scissors
    VkPipelineViewportStateCreateInfo infoViewport{
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = nullptr,
        .scissorCount  = 1,
        .pScissors     = nullptr,
    };

    VkPipelineRasterizationStateCreateInfo infoRasterization{
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = option.polygonMode,
        .cullMode                = option.cullMode,
        .frontFace               = option.frontFace,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo infoMultisample{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .rasterizationSamples  = option.rasterizationSamples,
        .sampleShadingEnable   = VK_TRUE,
        .minSampleShading      = 0.5f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE
    };

    VkPipelineDepthStencilStateCreateInfo infoDepthStencil{
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .depthTestEnable       = option.depthTestEnable,
        .depthWriteEnable      = option.depthWriteEnable,
        .depthCompareOp        = option.depthCompareOp,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f
    };

    // TODO: color blend state should be read from the input
    // Setup a dummy color blending for now
    VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable         = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo infoColorBlend{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_CLEAR,
        .attachmentCount = 1,
        .pAttachments    = &colorBlendAttachment
    };

    std::vector<VkDynamicState>      dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo infoDynamic{
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates    = dynamicStates.data()
    };

    // We don't support tessellation
    VkGraphicsPipelineCreateInfo infoPipeline{
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &infoRendering,
        .flags               = 0,
        .stageCount          = static_cast<uint32_t>(shaderStages.size()),
        .pStages             = shaderStages.data(),
        .pVertexInputState   = option.infoVertex,
        .pInputAssemblyState = &infoInputAssembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &infoViewport,
        .pRasterizationState = &infoRasterization,
        .pMultisampleState   = &infoMultisample,
        .pDepthStencilState  = &infoDepthStencil,
        .pColorBlendState    = &infoColorBlend,
        .pDynamicState       = &infoDynamic,
        .layout              = m_layout,
        .renderPass          = VK_NULL_HANDLE,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = 0,
    };

    DEBUG_VK_ASSERT(vkCreateGraphicsPipelines(VulkanState::GetInstance().GetDevice(), VK_NULL_HANDLE, 1, &infoPipeline, nullptr, &m_pipeline));
}
