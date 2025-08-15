#pragma once

#include <string>

#include <Singleton.h>
#include <include/ResourceManager.h>
#include <include/VulkanTexture.h>

class VulkanTexture;
class VulkanState;

class TextureManager
    : public Singleton<TextureManager>
    , public ResourceManager<TextureManager, VulkanTexture> {
public:
    void Init();

    void Destroy() { DestroyAll(); };

protected:
    TextureManager()  = default;
    ~TextureManager() = default;


    VulkanTexture CreateResource(const std::string &key, const SamplerConfig &config);

private:
    friend class ResourceManager<TextureManager, VulkanTexture>;
};