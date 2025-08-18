#include "include/MaterialRegistry.h"

#include <string>
#include <vector>

#include <include/ThreadPool.h>
#include <include/FileSystem.h>
#include <include/JsonInput.h>
#include <include/PipelineManager.h>
#include <include/TextureManager.h>
#include <include/VulkanGraphicsPipeline.h>

VulkanMaterial MaterialRegistry::CreateResource(const std::string &key) {
    file_system::MaterialConfig config(key);
    return VulkanMaterial(
        TextureManager::GetInstance().Load(config.albedo),
        TextureManager::GetInstance().Load(config.normal),
        TextureManager::GetInstance().Load(config.orm),
        TextureManager::GetInstance().Load(config.emissive),
        dynamic_cast<VulkanGraphicsPipeline *>(PipelineManager::GetInstance().Load(config.pipeline))
    );
}

void MaterialRegistry::Init() {
    std::vector<std::string> keys = file_system::GetFilesWithExtension("../Assets/Materials", ".json");

    for (const auto &key: keys) {
        Preload(key);
    }
}