#pragma once

#include <string>

#include <glm/glm.hpp>

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

    void CreateDefaultTexture(const std::string &key, const SamplerConfig &config, glm::vec4 color, VkFormat format);

private:
    friend class ResourceManager<TextureManager, VulkanTexture>;
};