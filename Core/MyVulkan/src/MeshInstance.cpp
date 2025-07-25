#include "include/MeshInstance.h"

#include <glm/gtx/euler_angles.hpp>

#include <include/VulkanMesh.h>
#include <include/VulkanGraphicsPipeline.h>
#include <include/VulkanUtil.h>

MeshInstance::MeshInstance(const VulkanMesh *mesh, const VulkanTexture *baseTexture,
                           std::shared_ptr<VulkanGraphicsPipeline> pipeline, VkDevice device,
                           VkDescriptorPool descriptorPool)
{
    m_mesh = mesh;
    m_baseTexture = baseTexture;
    m_pipeline = pipeline;
    m_device = device;
    m_descriptorPool = descriptorPool;
    CreateDescriptorSets();

    m_originalLocation = glm::vec3(0.f);
    Reset();
}

MeshInstance::MeshInstance(const VulkanMesh *mesh, const VulkanTexture *baseTexture,
                           std::shared_ptr<VulkanGraphicsPipeline> pipeline, VkDevice device,
                           VkDescriptorPool descriptorPool,
                           glm::vec3 location)
{
    m_mesh = mesh;
    m_baseTexture = baseTexture;
    m_pipeline = pipeline;
    m_device = device;
    m_descriptorPool = descriptorPool;
    CreateDescriptorSets();

    m_originalLocation = location;
    Reset();
}

MeshInstance::MeshInstance(const VulkanMesh *mesh, const VulkanTexture *baseTexture,
                           std::shared_ptr<VulkanGraphicsPipeline> pipeline, VkDevice device,
                           VkDescriptorPool descriptorPool,
                           glm::vec3 location,
                           glm::quat rotation, glm::vec3 scale)
{
    m_mesh = mesh;
    m_baseTexture = baseTexture;
    m_pipeline = pipeline;
    m_device = device;
    m_descriptorPool = descriptorPool;
    CreateDescriptorSets();

    m_originalLocation = location;
    m_location = m_originalLocation;
    m_rotation = rotation;
    m_scale = scale;
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    UpdateTransformation();
}

MeshInstance::MeshInstance(const VulkanMesh *mesh, const VulkanTexture *baseTexture,
                           std::shared_ptr<VulkanGraphicsPipeline> pipeline, VkDevice device,
                           VkDescriptorPool descriptorPool, glm::vec3 location,
                           glm::vec3 pitchYawRoll, glm::vec3 scale)
{
    m_mesh = mesh;
    m_baseTexture = baseTexture;
    m_pipeline = pipeline;
    m_device = device;
    m_descriptorPool = descriptorPool;
    CreateDescriptorSets();

    m_originalLocation = location;
    m_location = m_originalLocation;
    m_pitchYawRoll = pitchYawRoll;
    m_rotation = glm::quat_cast(glm::yawPitchRoll(m_pitchYawRoll.y, m_pitchYawRoll.x, m_pitchYawRoll.z));
    UpdateTransformation();
}

void MeshInstance::Destroy()
{
    if (m_device == VK_NULL_HANDLE)
    {
        return;
    }
    if (!m_descriptorSets.empty())
    {
        vkFreeDescriptorSets(m_device, m_descriptorPool, m_descriptorSets.size(), m_descriptorSets.data());
    }
    m_descriptorSets.clear();
    m_baseTexture = nullptr;
    m_mesh = nullptr;
}

void MeshInstance::Swap(MeshInstance &other) noexcept
{
    m_transformation = other.m_transformation;
    m_location = other.m_location;
    m_scale = other.m_scale;
    m_rotation = other.m_rotation;
    m_pitchYawRoll = other.m_pitchYawRoll;
    m_originalLocation = other.m_originalLocation;
    std::swap(m_mesh, other.m_mesh);
    std::swap(m_pipeline, other.m_pipeline);
    std::swap(m_device, other.m_device);
    std::swap(m_descriptorPool, other.m_descriptorPool);
    std::swap(m_descriptorSets, other.m_descriptorSets);
}

void MeshInstance::SetLocation(glm::vec3 location)
{
    m_location = location;
    UpdateTransformation();
}

void MeshInstance::SetScale(glm::vec3 scale)
{
    m_scale = scale;
    UpdateTransformation();
}

void MeshInstance::SetRotation(glm::quat rotation)
{
    m_rotation = rotation;
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    UpdateTransformation();
}

void MeshInstance::SetRotation(glm::vec3 pitchYawRoll)
{
    m_pitchYawRoll = pitchYawRoll;
    m_rotation = glm::quat_cast(glm::yawPitchRoll(m_pitchYawRoll.y, m_pitchYawRoll.x, m_pitchYawRoll.z));
    UpdateTransformation();
}


void MeshInstance::Reset()
{
    m_transformation = glm::mat4(1.0f);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(m_transformation, m_scale, m_rotation, m_location, skew, perspective);
    m_pitchYawRoll = glm::eulerAngles(m_rotation);
    m_location = m_originalLocation;

    UpdateTransformation();
}

void MeshInstance::UpdateTransformation()
{
    m_transformation = glm::translate(glm::mat4(1.0f), m_location) * glm::mat4_cast(m_rotation) * glm::scale(
                           glm::mat4(1.0f), m_scale);
}

void MeshInstance::CreateDescriptorSets()
{
    // The first set of a pipeline is always the uniform camera and light, which are handled in the VulkanState class
    for (size_t i = 1; i < m_pipeline->GetDescriptorSetLayouts().size(); i++)
    {
        VkDescriptorSet set = CreateDescriptorSet(m_pipeline->GetDescriptorSetLayouts()[i]);
        m_descriptorSets.push_back(set);
    }

    // TODO recfactor this
    UpdateDescriptorSets(m_descriptorSets[0], m_baseTexture);
}


VkDescriptorSet MeshInstance::CreateDescriptorSet(const VkDescriptorSetLayout &layout)
{
    VkDescriptorSet set = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo infoSet
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = m_descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout
    };

    DEBUG_VK_ASSERT(vkAllocateDescriptorSets(m_device, &infoSet, &set));

    return set;
}

void MeshInstance::UpdateDescriptorSets(VkDescriptorSet set, const VulkanTexture *texture)
{
    VkDescriptorImageInfo infoImage
    {
        .sampler = texture->GetSampler(),
        .imageView = texture->GetImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkWriteDescriptorSet writeSet
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = set,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &infoImage,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &writeSet, 0, nullptr);
}

void MeshInstance::BindAndDraw(VkCommandBuffer cmdBuf) const
{
    const VkDeviceSize offset = 0;

    if (!m_descriptorSets.empty())
    {
        vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetLayout(), 1,
                                m_descriptorSets.size(), m_descriptorSets.data(), 0, nullptr);
    }
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, &m_mesh->GetVertexBuffer(), &offset);
    vkCmdPushConstants(cmdBuf, m_pipeline->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(m_transformation), &m_transformation);
    vkCmdDraw(cmdBuf, m_mesh->GetVertexCount(), 1, 0, 0);
}
