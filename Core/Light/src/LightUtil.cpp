#include "include/LightUtil.h"

#include <array>

namespace {
glm::vec3 GetFrustumCenter(const std::array<glm::vec3, FRUSTUM_CORNER_NUM> &frustumCorners) {
    glm::vec3 center = glm::vec3(0.0f);

    for (const auto &corner: frustumCorners) {
        center += corner;
    }
    center /= frustumCorners.size();

    return center;
}

glm::mat4 GetLightProjectionMatrix(
    const std::array<glm::vec3, FRUSTUM_CORNER_NUM> &frustumCorners,
    const glm::mat4                                 &lightView,
    const std::array<glm::vec3, FRUSTUM_CORNER_NUM>  cameraFrustum
) {
    constexpr float fMax = std::numeric_limits<float>::max();
    constexpr float fMin = std::numeric_limits<float>::lowest();

    float minX = fMax;
    float maxX = fMin;
    float minY = fMax;
    float maxY = fMin;
    float minZ = fMax;
    float maxZ = fMin;

    for (const auto &corner: frustumCorners) {
        const glm::vec4 lightSpaceCorner = lightView * glm::vec4(corner, 1.0f);

        minX = glm::min(minX, lightSpaceCorner.x);
        maxX = glm::max(maxX, lightSpaceCorner.x);
        minY = glm::min(minY, lightSpaceCorner.y);
        maxY = glm::max(maxY, lightSpaceCorner.y);
        minZ = glm::min(minZ, lightSpaceCorner.z);
        maxZ = glm::max(maxZ, lightSpaceCorner.z);
    }

    for (const auto &corner: cameraFrustum) {
        const glm::vec4 worldCorner = lightView * glm::vec4(corner, 1.0f);
        minZ                        = glm::min(minZ, worldCorner.z);
        maxZ                        = glm::max(maxZ, worldCorner.z);
    }

    return glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
}
} // namespace

glm::mat4 light_util::GetLightSpaceMatrix(
    const glm::vec3                                 &lightDir,
    const std::array<glm::vec3, FRUSTUM_CORNER_NUM> &frustumCorners,
    const std::array<glm::vec3, FRUSTUM_CORNER_NUM> &cameraFrustum
) {
    static constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 center = GetFrustumCenter(frustumCorners);

    glm::mat4 view = glm::lookAt(center, center + lightDir, up);

    // TODO: currently we are using the corners of the entire frustum of the camera,
    // however, this ignores the shadow cast by the objects outside the frustum.
    // We need to expand the corners a bit
    glm::mat4 projection = GetLightProjectionMatrix(frustumCorners, view, cameraFrustum);

    return projection * view;
}
