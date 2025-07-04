#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vulkan/vulkan.h>

// Vertex with only 3D position
struct VertexP
{
    glm::vec3 position;

    VertexP() = default;

    explicit VertexP(const glm::vec3 &position) : position(position) {}

    static const VkPipelineVertexInputStateCreateInfo *GetVertexInputStateCreateInfo();
};

// Vertex with 3D positon, normal and texture coordinate
struct VertexPNT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    VertexPNT() = default;

    explicit VertexPNT(const glm::vec3 &position, const glm::vec3 &normal,
                       const glm::vec2 &texCoords) : position(position), normal(normal), texCoords(texCoords) {}

    static const VkPipelineVertexInputStateCreateInfo *GetVertexInputStateCreateInfo();
};
