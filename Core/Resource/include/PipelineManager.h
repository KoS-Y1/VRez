#pragma once

#include <memory>

#include "ResourceManager.h"
#include <Singleton.h>
#include <include/VulkanPipeline.h>

class GraphicsPipelineOption;

class PipelineManager
    : public Singleton<PipelineManager>
    , public ResourceManager<PipelineManager, std::unique_ptr<VulkanPipeline>> {
public:
    void Init();

    void Destroy() { DestroyAll(); }

protected:
    PipelineManager()  = default;
    ~PipelineManager() = default;

    std::unique_ptr<VulkanPipeline> CreateResource(
        const std::string             &key,
        const std::vector<std::string> files,
        const GraphicsPipelineOption  &option
    );
    std::unique_ptr<VulkanPipeline> CreateResource(const std::string &key, const std::vector<std::string> files);

private:
    friend class ResourceManager<PipelineManager, std::unique_ptr<VulkanPipeline>>;
};