#version 450

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTexcoord;

layout (location = 0) out vec2 vTexcoord;

void main()
{
    gl_Position = vec4(inPosition, 0.0f, 1.0f);
    vTexcoord = vec2(inTexcoord.x, 1.0f - inTexcoord.y);
}