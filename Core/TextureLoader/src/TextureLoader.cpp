#include "include/TextureLoader.h"

#include <SDL3/SDL_log.h>
#include <stb_image.h>

#include <include/VulkanTexture.h>
#include <include/FileSystem.h>
#include <Debug.h>

void TextureLoader::Destroy()
{
    for (auto &[file, texture]: m_textures)
    {
        texture.Destroy();
    }

    m_textures.clear();
}


VulkanTexture *TextureLoader::LoadTexture(const std::string &file, VulkanState &state, const SamplerConfig &config)
{
    auto pair = m_textures.find(file);

    if (pair == m_textures.end())
    {
        SDL_Log("Loading texture from file %s", file.c_str());

        int width = 0, height = 0;
        const unsigned char *data = Load(file, state, &width, &height);
        pair = m_textures.emplace(file, VulkanTexture(state, width, height, VK_FORMAT_R8G8B8A8_SRGB,
                                                      sizeof(unsigned char) * 4, data, config)).first;
    }

    return &pair->second;
}

unsigned char *TextureLoader::Load(const std::string &file, VulkanState &state, int *width, int *height)
{
    int channels;
    unsigned char *data = stbi_load(file.c_str(), width, height, &channels, STBI_rgb_alpha);

    DEBUG_ASSERT(data != nullptr);

    return data;
}
