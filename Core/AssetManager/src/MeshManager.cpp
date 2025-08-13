#include "include/MeshManager.h"

#include <vector>

#include <SDL3/SDL.h>

#include <include/VulkanState.h>
#include <include/VertexFormats.h>
#include <include/MeshLoader.h>
#include <include/FileSystem.h>

VulkanMesh MeshManager::CreateResource(const std::string &key, VulkanState &state) {
    SDL_Log("Loading mesh from file %s", key.c_str());
    const std::vector<VertexPNTT> vertices = file_system::LoadMesh(key);
    return VulkanMesh(state, file_system::GetFileName(key), vertices.size(), sizeof(VertexPNTT), vertices.data());
}

