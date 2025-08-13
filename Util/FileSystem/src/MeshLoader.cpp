#include "include/MeshLoader.h"

#include <SDL3/SDL.h>
#include <glm/vec3.hpp>
#include <tiny_obj_loader.h>

std::vector<VertexPNTT> file_system::LoadMesh(const std::string &file) {
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(file)) {
        if (!reader.Error().empty()) {
            SDL_Log("Error parsing obj file: %s", reader.Error().c_str());
            exit(EXIT_FAILURE);
        }
    }

    if (!reader.Warning().empty()) {
        SDL_Log("Warning parsing obj file: %s", reader.Warning().c_str());
    }

    const tinyobj::attrib_t            &attrib    = reader.GetAttrib();
    const std::vector<tinyobj::real_t> &positions = attrib.vertices;
    const std::vector<tinyobj::real_t> &normals   = attrib.normals;
    const std::vector<tinyobj::real_t> &texCoords = attrib.texcoords;

    std::vector<VertexPNTT> vertices;

    // Loop over shapes
    for (const tinyobj::shape_t &shape: reader.GetShapes()) {
        const tinyobj::mesh_t &mesh = shape.mesh;
        SDL_Log("Loading model shape %s", shape.name.c_str());

        size_t i = 0;

        // Loop over polygons in the mesh
        for (const size_t f: mesh.num_face_vertices) {
            // Must be triangles
            DEBUG_ASSERT(f == 3);

            // Calculate tangent space
            std::vector<glm::vec3> vPos;
            std::vector<glm::vec2> UVs;
            for (size_t v = 0; v < f; ++v) {
                const tinyobj::index_t &idx = mesh.indices[i + v];
                DEBUG_ASSERT(idx.texcoord_index >= 0);

                vPos.push_back(
                    glm::vec3{-positions[3 * idx.vertex_index + 0], positions[3 * idx.vertex_index + 1], positions[3 * idx.vertex_index + 2]}
                );

                UVs.push_back(glm::vec2{texCoords[2 * idx.texcoord_index + 0], texCoords[2 * idx.texcoord_index + 1]});
            }

            glm::vec3 deltaPos1 = vPos[1] - vPos[0];
            glm::vec3 deltaPos2 = vPos[2] - vPos[0];
            glm::vec2 deltaUV1  = UVs[1] - UVs[0];
            glm::vec2 deltaUV2  = UVs[2] - UVs[0];
            float     r         = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

            // Loop over vertices in the polygon
            for (size_t v = 0; v < f; ++v) {
                const tinyobj::index_t &idx = mesh.indices[i + v];

                // Check if `normal_index` is zero or positive. negative = no normal data
                DEBUG_ASSERT(idx.normal_index >= 0);
                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                DEBUG_ASSERT(idx.texcoord_index >= 0);

                // X axis is flipped Blender uses right-handed coordinates
                vertices.emplace_back(
                    glm::vec3{-positions[3 * idx.vertex_index + 0], positions[3 * idx.vertex_index + 1], positions[3 * idx.vertex_index + 2]},
                    glm::vec3{-normals[3 * idx.normal_index + 0], normals[3 * idx.normal_index + 1], normals[3 * idx.normal_index + 2]},
                    tangent,
                    glm::vec2{texCoords[2 * idx.texcoord_index + 0], texCoords[2 * idx.texcoord_index + 1]}
                );
            }

            i += f;
        }
    }

    return vertices;
}
