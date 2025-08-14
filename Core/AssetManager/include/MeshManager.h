#pragma once

#include <string>

#include <Singleton.h>
#include <include/ResourceManager.h>
#include <include/VulkanMesh.h>

class VulkanState;

class MeshManager
    : public Singleton<MeshManager>
    , public ResourceManager<MeshManager, std::string, VulkanMesh> {
    public:
    VulkanMesh *LoadMesh(const std::string &key) { return Load(key); }

    void Init();

    void Destroy() { DestroyAll(); };

protected:
    MeshManager()  = default;
    ~MeshManager() = default;

    VulkanMesh CreateResource(const std::string &key);
private:
    friend class ResourceManager<MeshManager, std::string, VulkanMesh>;
};