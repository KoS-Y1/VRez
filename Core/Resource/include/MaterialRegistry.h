#pragma once

#include "ResourceManager.h"
#include "VulkanMaterial.h"
#include <Singleton.h>

class MaterialRegistry
    : public Singleton<MaterialRegistry>
    , public ResourceManager<MaterialRegistry, VulkanMaterial> {
public:
    void Init();

    void Destroy() { DestroyAll(); };

protected:
    MaterialRegistry()  = default;
    ~MaterialRegistry() = default;


private:
    friend class ResourceManager<MaterialRegistry, VulkanMaterial>;
    VulkanMaterial CreateResource(const std::string &key);
};