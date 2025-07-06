#version 450

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}