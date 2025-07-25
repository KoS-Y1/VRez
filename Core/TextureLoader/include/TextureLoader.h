#pragma once

#include <vector>
#include <string>
#include <map>

#include "Singleton.h"
#include "include/VulkanTexture.h"

class VulkanTexture;
class VulkanState;
struct SamplerConfig;

class TextureLoader : public Singleton<TextureLoader>
{
public:
    void Destroy();

    VulkanTexture *LoadTexture(const std::string &file, VulkanState &state, const SamplerConfig &config);

protected:
    TextureLoader() = default;

    ~TextureLoader() {Destroy();}

private:
    std::map<std::string, VulkanTexture> m_textures;

    unsigned char *Load(const std::string &file, VulkanState &state, int *width, int *height);
};
