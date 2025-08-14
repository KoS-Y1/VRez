#include "include/TextureManager.h"

#include <SDL3/SDL.h>

#include <include/TextureLoader.h>
#include <include/VulkanState.h>
#include <include/ThreadPool.h>

VulkanTexture TextureManager::CreateResource(const std::string &key, const SamplerConfig &config) {
    SDL_Log("Loading texture from file %s", key.c_str());

    int                  width  = 0;
    int                  height = 0;
    const unsigned char *data   = file_system::LoadTexture(key, &width, &height);

    return VulkanTexture(width, height, VK_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned char) * 4, data, config);
}

void TextureManager::Init() {
    std::vector<std::string> keys{
        "../Assets/Models/Skybox/Skybox.png",
        "../Assets/Models/Skybox/specular.png",
        "../Assets/Models/Skybox/irradiance.png",
        "../Assets/Models/Skybox/brdf_lut.png",
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
    for (const auto &key : keys) {
        ThreadPool::GetInstance().Enqueue([this, key, config]() {
         Preload(key, config);
     });
    }
}
