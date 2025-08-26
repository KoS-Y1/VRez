#version 450

#include <uniform_lights.glsl>

layout (location = 0) in vec3 gWorldNormal;

void main()
{
    const vec3 lightDirection = normalize(uLights[0].direction);
    const vec3 worldNormal = normalize(gWorldNormal);
    const float slopeScale = 1 - abs(dot(lightDirection, worldNormal));
    const float bias = float[](0.002, 0.004, 0.008)[gl_Layer];
    gl_FragDepth = gl_FragCoord.z + bias * slopeScale / gl_FragCoord.w;
}