#include "include/TextureManager.h"

#include <SDL3/SDL.h>

#include <include/TextureLoader.h>
#include <include/VulkanState.h>

VulkanTexture TextureManager::CreateResource(const std::string &key, VulkanState &state, const SamplerConfig &config) {
    SDL_Log("Loading texture from file %s", key.c_str());

    int                  width  = 0;
    int                  height = 0;
    const unsigned char *data   = file_system::LoadTexture(key, &width, &height);

    return VulkanTexture(width, height, VK_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned char) * 4, data, config);
}
