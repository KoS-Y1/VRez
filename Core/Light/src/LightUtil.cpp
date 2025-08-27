#include "include/LightUtil.h"

#include <array>

namespace {} // namespace

glm::mat4 light_util::GetLightSpaceMatrix(const glm::vec3 &lightDir) {
    static constexpr float near = -1.0f;
    static constexpr float far  = 5.0f;

    glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near, far);

    static constexpr glm::vec3 up        = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 center       = glm::vec3(0.0f);
    glm::mat4                  lightView = glm::lookAt(center - lightDir, center, up);

    return lightProjection * lightView;
}
