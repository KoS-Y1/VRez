#include "include/TextureManager.h"

#include "include/FileSystem.h"

#include <SDL3/SDL.h>

#include <include/TextureLoader.h>
#include <include/ThreadPool.h>
#include <include/VulkanState.h>

VulkanTexture TextureManager::CreateResource(const std::string &key, const SamplerConfig &config) {
    SDL_Log("Loading texture from file %s", key.c_str());

    int                  width  = 0;
    int                  height = 0;
    const unsigned char *data   = file_system::LoadTexture(key, &width, &height);

    return VulkanTexture(static_cast<uint32_t>(width), static_cast<uint32_t>(height), VK_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned char) * 4, data, config);
}

void TextureManager::Init() {
    std::vector<std::string> pngKeys = file_system::GetFilesWithExtension("../Assets", ".png");
    std::vector<std::string> jpgKeys = file_system::GetFilesWithExtension("../Assets", ".jpg");

    SamplerConfig config;
    for (const auto &key: pngKeys) {
        ThreadPool::GetInstance().Enqueue([this, key, config]() { Preload(key, config); });
    }
    for (const auto &key: jpgKeys) {
        ThreadPool::GetInstance().Enqueue([this, key, config]() { Preload(key, config); });
    }

    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("albedo", config, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), VK_FORMAT_R32G32B32A32_SFLOAT);
    });
    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("normal", config, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), VK_FORMAT_R32G32B32A32_SFLOAT);
    });
    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("orm", config, glm::vec4(0.2f, 1.0f, 0.1f, 1.0f), VK_FORMAT_R32G32B32A32_SFLOAT);
    });
    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("emissive", config, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), VK_FORMAT_R32G32B32A32_SFLOAT);
    });
}

void TextureManager::CreateDefaultTexture(const std::string &key, const SamplerConfig &config, glm::vec4 color, VkFormat format) {
    VulkanTexture texture = VulkanTexture(1u, 1u, format, 4 * sizeof(float), &color, config);
    {
        std::scoped_lock<std::mutex> lk(m_cacheMutex);
        m_cache.emplace(key, std::move(texture));
    }
}
