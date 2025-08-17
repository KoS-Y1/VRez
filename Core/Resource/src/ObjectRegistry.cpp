#include "include/ObjectRegistry.h"

#include <string>
#include <vector>

#include <include/FileSystem.h>
#include <include/JsonInput.h>
#include <include/MaterialRegistry.h>
#include <include/MeshManager.h>
#include <include/ThreadPool.h>
#include <include/VulkanObject.h>

VulkanObject ObjectRegistry::CreateResource(const std::string &key) {
    file_system::ObjectConfig config(key);
    return VulkanObject(MeshManager::GetInstance().Load(config.mesh), MaterialRegistry::GetInstance().Load(config.material));
}

void ObjectRegistry::Init() {
    std::vector<std::string> keys = file_system::GetFilesWithExtension("../Assets/Models", ".json");

    for (const auto &key: keys) {
        ThreadPool::GetInstance().Enqueue([this, key]() { Preload(key); });
    }
}
