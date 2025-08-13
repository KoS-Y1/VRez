#pragma once

#include <string>

#include <Singleton.h>
#include <include/ResourceManager.h>
#include <include/VulkanTexture.h>

class VulkanTexture;
class VulkanState;
class SamplerConfig;

class TextureManager
    : public Singleton<TextureManager>
    , public ResourceManager<TextureManager, std::string, VulkanTexture> {
public:
    VulkanTexture *LoadTexture(const std::string &key, VulkanState &state, const SamplerConfig &config) {
        return Load(key, std::move(state), std::move(config));
    }

    void Destroy() { DestroyAll(); };

protected:
    TextureManager()  = default;
    ~TextureManager() = default;

    VulkanTexture CreateResource(const std::string &key, VulkanState &state, const SamplerConfig &config);
};