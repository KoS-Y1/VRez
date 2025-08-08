#version 450

#include <uniform_camera.glsl>

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 vWorldPosition;

void main()
{
    mat4 view = uView;
    view[3] = vec4(0, 0, 0, 1);
    vec4 position = uProjection * view * vec4(inPosition, 1);

    vWorldPosition = inPosition;

    gl_Position = position.xyww;
}
