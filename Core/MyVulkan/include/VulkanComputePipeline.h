#pragma once
#include "VulkanPipeline.h"

#include <string>

class VulkanComputePipeline : public VulkanPipeline {
public:
    VulkanComputePipeline() = default;

    VulkanComputePipeline(const std::vector<std::string> &paths)
        : VulkanPipeline(paths) {
        CreatePipeline();
    }

protected:
    void CreatePipeline();

private:
    friend class VulkanPipeline;
};
