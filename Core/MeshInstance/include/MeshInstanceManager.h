#pragma once

#include <vector>

#include <Singleton.h>

#include <include/MeshInstance.h>
#include <include/VulkanTexture.h>

class MeshInstanceManager : public Singleton<MeshInstanceManager> {
  public:
    void Init(VulkanState &state);
    void Destroy();

    void Load();

  protected:
    MeshInstanceManager() = default;

    ~MeshInstanceManager() = default;

  private:
    std::vector<MeshInstance> m_meshInstances;

    // Default textures
    VulkanTexture m_albedo;
    VulkanTexture m_normal;
    VulkanTexture m_orm;
    VulkanTexture m_emissive;

    void CreateDefaultTextures(VulkanState &state);
};
