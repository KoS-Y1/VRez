#include "include/MeshInstanceManager.h"

#include <mutex>

#include <glm/glm.hpp>

#include <include/Job.h>

#include <include/VulkanTexture.h>

struct LoadInstanceJob : public Job
{
    LoadInstanceJob(std::mutex )
    {}

private:
    std::mutex *m_mutex;
};

void MeshInstanceManager::Init(VulkanState &state)
{
    CreateDefaultTextures(state);
}

void MeshInstanceManager::Destroy()
{
    for (auto &instance: m_meshInstances)
    {
        instance.Destroy();
    }

    m_albedo.Destroy();
    m_normal.Destroy();
    m_orm.Destroy();
    m_emissive.Destroy();
}

void MeshInstanceManager::CreateDefaultTextures(VulkanState &state)
{
    // Base texture for instance that does not have an input color texture
    SamplerConfig samplerConfig;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    VulkanTexture texture(state, 1u, 1u, VK_FORMAT_R32G32B32A32_SFLOAT, 4 * sizeof(float), &color, samplerConfig);
    m_albedo = std::move(texture);

    // A default flat normal map
    glm::vec4 normal = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
    VulkanTexture tempNormal(state, 1u, 1u, VK_FORMAT_R32G32B32A32_SFLOAT, 4 * sizeof(float), &normal, samplerConfig);
    m_normal = std::move(tempNormal);

    // A default orm map
    glm::vec4 orm = glm::vec4(0.2f, 1.0f, 0.1f, 1.0f);
    VulkanTexture ormTexture(state, 1u, 1u, VK_FORMAT_R32G32_SFLOAT, 4 * sizeof(float), &orm, samplerConfig);
    m_orm = std::move(ormTexture);

    // A default emissive map
    glm::vec4 emissive = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    VulkanTexture emissiveTexture(state, 1u, 1u, VK_FORMAT_R32G32_SFLOAT, 4 * sizeof(float), &emissive, samplerConfig);
    m_emissive = std::move(emissiveTexture);
}
