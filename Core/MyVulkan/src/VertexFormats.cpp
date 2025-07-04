#include "include/VertexFormats.h"

const VkPipelineVertexInputStateCreateInfo *VertexP::GetVertexInputStateCreateInfo()
{
    static const std::vector<VkVertexInputBindingDescription> bindingDescriptions
    {
        {
            .binding = 0,
            .stride = sizeof(VertexP),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };

    static const std::vector<VkVertexInputAttributeDescription> attributeDescriptions
    {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexP, position)
        }
    };

    static const VkPipelineVertexInputStateCreateInfo infoVertex
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size()),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    return &infoVertex;
}

const VkPipelineVertexInputStateCreateInfo *VertexPNT::GetVertexInputStateCreateInfo()
{
    static const std::vector<VkVertexInputBindingDescription> bindingDescriptions
    {
        {
            .binding = 0,
            .stride = sizeof(VertexPNT),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };

    static const std::vector<VkVertexInputAttributeDescription> attributeDescriptions
    {
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexPNT, position)
        },
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(VertexPNT, normal)
        },
        {
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(VertexPNT, texCoords)
        }
    };

    static const VkPipelineVertexInputStateCreateInfo infoVertex
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size()),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    return &infoVertex;
}
