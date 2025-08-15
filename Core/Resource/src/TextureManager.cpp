#include "include/TextureManager.h"

#include <SDL3/SDL.h>

#include <include/TextureLoader.h>
#include <include/ThreadPool.h>
#include <include/VulkanState.h>

VulkanTexture TextureManager::CreateResource(const std::string &key, const SamplerConfig &config) {
    SDL_Log("Loading texture from file %s", key.c_str());

    int                  width  = 0;
    int                  height = 0;
    const unsigned char *data   = file_system::LoadTexture(key, &width, &height);

    return VulkanTexture(width, height, VK_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned char) * 4, data, config);
}

void TextureManager::Init() {
    std::vector<std::string> keys{
        "../Assets/Skybox/Skybox.png",
        "../Assets/Skybox/specular.png",
        "../Assets/Skybox/irradiance.png",
        "../Assets/Skybox/brdf_lut.png",
        "../Assets/Models/Chessboard/chessboard_base_color.jpg",
        "../Assets/Models/Castle/castle_white_base_color.jpg",
        "../Assets/Models/BoomBox/BoomBox_baseColor.png",
        "../Assets/Models/Chessboard/chessboard_normal.jpg",
        "../Assets/Models/Castle/Castle_normal.jpg",
        "../Assets/Models/BoomBox/BoomBox_normal.png",
        "../Assets/Models/Chessboard/Chessboard_ORM.jpg",
        "../Assets/Models/Castle/Castle_ORM.jpg",
        "../Assets/Models/BoomBox/BoomBox_occlusionRoughnessMetallic.png",
        "../Assets/Models/BoomBox/BoomBox_emissive.png",
    };
    SamplerConfig config;
    for (const auto &key: keys) {
        ThreadPool::GetInstance().Enqueue([this, key, config]() { Preload(key, config); });
    }

    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("albedo", config, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), VK_FORMAT_R32G32B32A32_SFLOAT);
    });
    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("normal", config, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f), VK_FORMAT_R32G32B32A32_SFLOAT);
    });
    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("orm", config, glm::vec4(0.2f, 1.0f, 0.1f, 1.0f), VK_FORMAT_R32G32_SFLOAT);
    });
    ThreadPool::GetInstance().Enqueue([this, config]() {
        CreateDefaultTexture("emissive", config, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), VK_FORMAT_R32G32_SFLOAT);
    });
}

void TextureManager::CreateDefaultTexture(const std::string &key, const SamplerConfig &config, glm::vec4 color, VkFormat format) {
    VulkanTexture texture = VulkanTexture(1u, 1u, format, 4 * sizeof(float), &color, config);
    {
        std::scoped_lock<std::mutex> lk(m_cacheMutex);
        m_cache.emplace(key, std::move(texture));
    }
}
