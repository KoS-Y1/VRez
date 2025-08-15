#pragma once

#include <string>

#include <Singleton.h>
#include <include/ResourceManager.h>
#include <include/VulkanMesh.h>

class VulkanState;

class MeshManager
    : public Singleton<MeshManager>
    , public ResourceManager<MeshManager, VulkanMesh> {
public:
    void Init();

    void Destroy() { DestroyAll(); };

protected:
    MeshManager()  = default;
    ~MeshManager() = default;

    VulkanMesh CreateResource(const std::string &key);

private:
    friend class ResourceManager<MeshManager, VulkanMesh>;
};