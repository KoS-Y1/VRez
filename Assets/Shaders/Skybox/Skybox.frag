#version 450

#include <util.glsl>

layout (location = 0) in vec3 vWorldPosition;

layout (set = TEXTURE_SET, binding = 0) uniform sampler2D emissive;

layout (location = 0) out vec4 outColor;

void main()
{
    vec2 uv = SampleSphericalMap(vWorldPosition);
    outColor = RGBMToLinear(texture(emissive, uv));
}
