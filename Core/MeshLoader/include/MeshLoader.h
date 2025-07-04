#pragma once

#include <map>

#include "include/VulkanMesh.h"
#include "include/VertexFormats.h"

class MeshLoader
{
public:
    MeshLoader() = delete;

    MeshLoader(VulkanState &state) : m_state(state) {}

    ~MeshLoader() = default;

    MeshLoader(const MeshLoader&) = delete;
    MeshLoader(MeshLoader&&) = delete;

    MeshLoader& operator=(const MeshLoader&) = delete;
    MeshLoader& operator=(MeshLoader&&) = delete;

    VulkanMesh *LoadMesh(const std::string &file);

private:
    VulkanState &m_state;
    std::map<std::string, VulkanMesh> m_meshes;

    std::vector<VertexPNT> Load(const std::string file);
};
