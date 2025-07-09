#pragma once

#include <map>

#include "include/VulkanMesh.h"
#include "include/VertexFormats.h"
#include "include/VulkanState.h"

class MeshLoader
{
public:
    MeshLoader() = default;

    ~MeshLoader() { Destroy(); };

    MeshLoader(const MeshLoader&) = delete;
    MeshLoader(MeshLoader&&) = delete;

    MeshLoader& operator=(const MeshLoader&) = delete;
    MeshLoader& operator=(MeshLoader&&) = delete;

    void Destroy();

    VulkanMesh *LoadMesh(const std::string &file, VulkanState &state);

private:
    std::map<std::string, VulkanMesh> m_meshes;

    std::vector<VertexPNT> Load(const std::string file);

};
