#version 450

#include <uniform_camera.glsl>

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 vWorldPosition;

void main()
{
    vWorldPosition = inPosition;

    gl_Position = uProjection * uView * vec4(inPosition, 1.0f);
}