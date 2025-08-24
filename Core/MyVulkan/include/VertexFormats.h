#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vulkan/vulkan.h>

// Vertex with only 3D position
struct VertexP {
    glm::vec3 position;

    VertexP() = default;

    explicit VertexP(const glm::vec3 &position)
        : position(position) {}

    static const VkPipelineVertexInputStateCreateInfo *GetVertexInputStateCreateInfo();
};

// Vertex with 3D positon, normal and texture coordinate
struct VertexPNT {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    VertexPNT() = default;

    VertexPNT(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texCoords)
        : position(position)
        , normal(normal)
        , texCoords(texCoords) {}

    static const VkPipelineVertexInputStateCreateInfo *GetVertexInputStateCreateInfo();
};

// Vertex with 3D positon, normal ,tangent, and texture coordinate
struct VertexPNTT {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoords;

    VertexPNTT() = default;

    VertexPNTT(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec3 &tangent, const glm::vec2 texCoord)
        : position(position)
        , normal(normal)
        , tangent(tangent)
        , texCoords(texCoord) {}

    static const VkPipelineVertexInputStateCreateInfo *GetVertexInputStateCreateInfo();
};

struct VertexPT2D {
    glm::vec2 position;
    glm::vec2 texCoords;

    VertexPT2D() = default;

    VertexPT2D(const glm::vec2 &position, const glm::vec2 &texCoords) : position(position), texCoords(texCoords) {}

    static const VkPipelineVertexInputStateCreateInfo *GetVertexInputStateCreateInfo();
};