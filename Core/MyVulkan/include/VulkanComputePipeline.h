#pragma once
#include "VulkanPipeline.h"

#include <string>

class VulkanComputePipeline : public VulkanPipeline {
public:
    VulkanComputePipeline() = delete;

    explicit VulkanComputePipeline(const std::vector<std::string> &paths)
        : VulkanPipeline(paths) {
        CreatePipeline();
    }

protected:
    void CreatePipeline();
};
