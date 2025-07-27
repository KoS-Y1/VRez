#version 450

#include <uniform_lights.glsl>
#include <uniform_camera.glsl>
#include <pbr.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in mat3 vTBN;
layout (location = 4) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D uBaseTexture;
layout (set = 1, binding = 1) uniform sampler2D uNormalMap;
layout (set = 1, binding = 2) uniform sampler2D uORMTexture;

void main()
{
    int i = 0;

    vec3 albedo = texture(uBaseTexture, vTexcoord).xyz;
    vec3 Lo = vec3(0.0f);

    vec3 tNormal = texture(uNormalMap, vTexcoord).xyz * 2.0 - 1.0;
    vec3 normal = normalize(vTBN * tNormal);

    vec3 orm = texture(uORMTexture, vTexcoord).xyz;
    // TODO: use ao when there's ambient light/IBL
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;

    for (i = 0; i < uLightCount; ++i)
    {
        vec3 viewDir = normalize(uViewPosition - vWorldPosition);

        Lo += CalculatePBRLight(uLights[i].position, uLights[i].direction, uLights[i].color, uLights[i].intensity,
        uLights[i].range, uLights[i].type, normal, viewDir, vWorldPosition, albedo, roughness, metallic);
    }

    // Gamma correction
    outColor = vec4(pow(Lo, vec3(1.0/2.2)), 1.0f);
}