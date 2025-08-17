#pragma once

#include "ResourceManager.h"
#include "VulkanObject.h"
#include <Singleton.h>

class ObjectRegistry
    : public Singleton<ObjectRegistry>
    , public ResourceManager<ObjectRegistry, VulkanObject> {
public:
    void Init();

    void Destroy() { DestroyAll(); }

protected:
    ObjectRegistry() = default;
    ~ObjectRegistry() = default;

    VulkanObject CreateResource(const std::string &key);

private:
    friend class ResourceManager<ObjectRegistry, VulkanObject>;
};