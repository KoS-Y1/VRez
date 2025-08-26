#include "include/LightUtil.h"

#include <array>

namespace {
} // namespace

glm::mat4 light_util::GetLightSpaceMatrix(
    const glm::vec3                                 &lightDir
) {
    static constexpr float near = 0.001f;
    static constexpr float far = 32.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);

    static constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 pos = glm::vec3(-2.0f, 5.0f, 2.0f);
    glm::mat4 lightView = glm::lookAt(pos, pos + lightDir, up);

    return lightProjection * lightView;
}
