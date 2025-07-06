#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoord;

layout (location = 0) out vec3 vWorldPositon;
layout (location = 1) out vec3 vWorldNormal;
layout (location = 2) out vec2 vTexcoord;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);

    vWorldPositon = inPosition;
    vWorldNormal = inNormal;
    vTexcoord = inTexcoord;
}
