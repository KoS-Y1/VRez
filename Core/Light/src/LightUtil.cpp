#include "include/LightUtil.h"

#include <array>

namespace {} // namespace

glm::mat4 light_util::GetLightSpaceMatrix(const glm::vec3 &lightDir) {
    static constexpr float near = -2.0f;
    static constexpr float far  = 2.0f;

    glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, near, far);

    static constexpr glm::vec3 up        = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 center       = glm::vec3(0.0f);
    glm::mat4                  lightView = glm::lookAt(center - lightDir, center, up);

    return lightProjection * lightView;
}
