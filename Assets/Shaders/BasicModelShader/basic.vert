#version 450

#include <uniform_camera.glsl>

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoord;

layout (location = 0) out vec3 vWorldPositon;
layout (location = 1) out vec3 vWorldNormal;
layout (location = 2) out vec2 vTexcoord;


layout (push_constant) uniform PushConstantData
{
    mat4 inModel;
};

void main()
{

    vWorldPositon = (inModel * vec4(inPosition, 1.0f)).xyz;
    vWorldNormal = (inModel * vec4(inNormal, 1.0f)).xyz;
    vTexcoord = inTexcoord;

    gl_Position = uView * vec4(vWorldPositon, 1.0f);
}
