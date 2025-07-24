#pragma once

#include <map>

#include "include/VulkanMesh.h"
#include "include/VertexFormats.h"
#include "include/VulkanState.h"
#include <Singleton.h>

class MeshLoader : public Singleton<MeshLoader>
{
public:
    void Destroy();

    VulkanMesh *LoadMesh(const std::string &file, VulkanState &state);

protected:
    MeshLoader() = default;

    ~MeshLoader() { Destroy(); };

private:
    std::map<std::string, VulkanMesh> m_meshes;

    std::vector<VertexPNT> Load(const std::string &file);
};
