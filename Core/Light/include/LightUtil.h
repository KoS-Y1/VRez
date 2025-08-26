#pragma once

#include <glm/glm.hpp>

#include <include/Camera.h>

namespace light_util {
glm::mat4 GetLightSpaceMatrix(
    const glm::vec3                                 &lightDir,
    const std::array<glm::vec3, FRUSTUM_CORNER_NUM> &frustumCorners,
    const std::array<glm::vec3, FRUSTUM_CORNER_NUM> &cameraFrustum
);
} // namespace light_util
