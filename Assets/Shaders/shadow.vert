#version 450

#include <uniform_camera.glsl>
#include <uniform_lights.glsl>

layout (location = 0) in vec3 inPositon;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

layout (push_constant) uniform PushConstantData
{
    mat4 inModel;
};

void main()
{
    gl_Position = uLightSpaceMatrix * inModel * vec4(inPositon, 1.0f);
}