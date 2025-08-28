#version 450

#include <util.glsl>
#include <uniform_lights.glsl>
#include <uniform_camera.glsl>
#include <pbr.glsl>
#include <ibl.glsl>
#include <shadow.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in mat3 vTBN;
layout (location = 4) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = TEXTURE_SET, binding = 0) uniform sampler2D uAlbedo;
layout (set = TEXTURE_SET, binding = 1) uniform sampler2D uNormal;
layout (set = TEXTURE_SET, binding = 2) uniform sampler2D uORM;
layout (set = TEXTURE_SET, binding = 3) uniform sampler2D uEmissive;

void main()
{
    const vec3 albedo = texture(uAlbedo, vTexcoord).xyz;
    const vec3 tNormal = texture(uNormal, vTexcoord).xyz;
    const vec3 worldNormal = normalize(vTBN * tNormal);
    const vec3 orm = texture(uORM, vTexcoord).xyz;
    const vec3 emissive = texture(uEmissive, vTexcoord).xyz;

    const float ao = orm.r;
    const float roughness = orm.g;
    const float metallic = orm.b;

    const vec3 N = worldNormal;
    const vec3 V = normalize(uViewPosition - vWorldPosition);
    const vec3 R = reflect(-V, N);
    const float NdotV = max(dot(N, V), 0.0f);
    const vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    // PBR
    vec3 Lo = vec3(0.0f);
    for(int i = 1; i < uLightCount; ++i)
    {
        Lo += CalculatePBRLight(uLights[i].position, uLights[i].direction, uLights[i].color, uLights[i].intensity,
        uLights[i].range, uLights[i].type, N, V, vWorldPosition, albedo, roughness, metallic);
    }

    const float shadow = ReadShadowMap(vWorldPosition, N);
    Lo += CalculatePBRLight(uLights[0].position, uLights[0].direction, uLights[0].color, uLights[0].intensity,
    uLights[0].range, uLights[0].type, N, V, vWorldPosition, albedo, roughness, metallic) * (1.0f - shadow);

    // IBL
    vec3 ibl = IBL(N, NdotV, R, F0, metallic, roughness, albedo, ao) * 0.5;

    outColor = vec4((ibl+ Lo + emissive), 1.0);
}