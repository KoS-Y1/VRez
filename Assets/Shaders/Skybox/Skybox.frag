#version 450

#include <util.glsl>

layout (location = 0) in vec3 vWorldPosition;

layout (set = 1, binding = 0) uniform sampler2D sphereMap;

layout (location = 0) out vec4 outColor;


void main()
{
    vec2 uv = SampleSphericalMap(vWorldPosition);

    outColor = RGBMToLinear(texture(sphereMap, uv));
}