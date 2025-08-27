#include "include/LightUtil.h"

#include <array>

namespace {} // namespace

glm::mat4 light_util::GetLightSpaceMatrix(const glm::vec3 &lightDir) {
    static constexpr float halfExtent = 2.0f;
    static constexpr float pullback   = halfExtent * 2.0f;
    static constexpr float near       = 0.1f;
    static constexpr float far        = halfExtent * 2.0f + pullback;

    static constexpr glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 center = glm::vec3(0.0f);

    glm::vec3 eye = center - lightDir * pullback;

    glm::mat4 lightProjection = glm::ortho(-halfExtent, halfExtent, -halfExtent, halfExtent, near, far);
    glm::mat4 lightView       = glm::lookAt(eye, center, up);

    return lightProjection * lightView;
}
