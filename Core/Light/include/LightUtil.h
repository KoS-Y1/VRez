#pragma once

#include <glm/glm.hpp>

#include <include/Camera.h>

namespace light_util {
glm::mat4 GetLightSpaceMatrix(
    const glm::vec3                                 &lightDir
);
} // namespace light_util
