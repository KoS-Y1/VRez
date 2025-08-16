#include "include/MeshManager.h"

#include <vector>

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <include/FileSystem.h>
#include <include/MeshLoader.h>
#include <include/ThreadPool.h>
#include <include/VertexFormats.h>
#include <include/VulkanState.h>

VulkanMesh MeshManager::CreateResource(const std::string &key) {
    SDL_Log("Loading mesh from file %s", key.c_str());
    const std::vector<VertexPNTT> vertices = file_system::LoadMesh(key);
    return VulkanMesh(file_system::GetFileName(key), vertices.size(), sizeof(VertexPNTT), vertices.data());
}

void MeshManager::Init() {
    std::vector<std::string> keys = file_system::GetFilesWithExtension("../Assets/Models", ".obj");

    for (const auto &key: keys) {
        ThreadPool::GetInstance().Enqueue([this, key]() { Preload(key); });
    }

    ThreadPool::GetInstance().Enqueue([this]() {
        CreateSkyboxMesh();
    });
}

void MeshManager::CreateSkyboxMesh() {
    std::vector<VertexP> vertices{
        VertexP(glm::vec3(-1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(-1.0f, -1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, -1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, -1.0f, -1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, -1.0f)),
        VertexP(glm::vec3(-1.0f, 1.0f, 1.0f)),
        VertexP(glm::vec3(-1.0f, 1.0f, -1.0f)),
        VertexP(glm::vec3(-1.0f, -1.0f, 1.0f)),
        VertexP(glm::vec3(-1.0f, -1.0f, -1.0f)),
        VertexP(glm::vec3(1.0f, -1.0f, -1.0f)),
        VertexP(glm::vec3(-1.0f, 1.0f, -1.0f)),
        VertexP(glm::vec3(1.0f, 1.0f, -1.)),
    };

    VulkanMesh mesh = VulkanMesh("skybox", vertices.size(), sizeof(VertexP), vertices.data());

    {
        std::scoped_lock<std::mutex> lk(m_cacheMutex);
        m_cache.emplace("skybox", std::move(mesh));
    }
}