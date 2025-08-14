#include "include/MeshManager.h"

#include <vector>

#include <SDL3/SDL.h>

#include <include/FileSystem.h>
#include <include/MeshLoader.h>
#include <include/VertexFormats.h>
#include <include/VulkanState.h>

VulkanMesh MeshManager::CreateResource(const std::string &key) {
    SDL_Log("Loading mesh from file %s", key.c_str());
    const std::vector<VertexPNTT> vertices = file_system::LoadMesh(key);
    return VulkanMesh(file_system::GetFileName(key), vertices.size(), sizeof(VertexPNTT), vertices.data());
}

void MeshManager::Init() {
    std::vector<std::string> keys
    {
        "../Assets/Models/Chessboard/Chessboard.obj",
        "../Assets/Models/Castle/Castle.obj",
        "../Assets/Models/BoomBox/BoomBox.obj",
    };

    for (const auto &key : keys) {
        Preload(key);
    }
}
