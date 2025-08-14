#version 450

#include <uniform_lights.glsl>
#include <uniform_camera.glsl>
#include <pbr.glsl>
#include <ibl.glsl>
#include <csm.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in mat3 vTBN;
layout (location = 4) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D uBaseTexture;
layout (set = 1, binding = 1) uniform sampler2D uNormalMap;
layout (set = 1, binding = 2) uniform sampler2D uORMTexture;
layout (set = 1, binding = 3) uniform sampler2D uEmissiveTexture;

void main()
{
    int i = 0;

    vec3 albedo = texture(uBaseTexture, vTexcoord).xyz;
    vec3 Lo = vec3(0.0f);

    vec3 tNormal = texture(uNormalMap, vTexcoord).xyz * 2.0 - 1.0;
    vec3 normal = normalize(vTBN * tNormal);

    vec3 orm = texture(uORMTexture, vTexcoord).xyz;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;

    vec4 viewSpacePos = uView * vec4(vWorldPosition, 1.0f);

    // PBR
    for (i = 0; i < uLightCount; ++i)
    {
        vec3 viewDir = normalize(uViewPosition - vWorldPosition);

        Lo += CalculatePBRLight(uLights[i].position, uLights[i].direction, uLights[i].color, uLights[i].intensity,
        uLights[i].range, uLights[i].type, normal, viewDir, vWorldPosition, albedo, roughness, metallic);
    }

    vec3 emissive = texture(uEmissiveTexture, vTexcoord).rgb;

    vec3 N = normal;
    vec3 V = normalize(uViewPosition - vWorldPosition);
    vec3 R = reflect(-V, N);
    float NdotV = max(dot(N, V), 0.0f);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    // IBL
    vec3 ibl = IBL(N, NdotV, R, F0, metallic, roughness, albedo, ao);

    // Gamma correction
//    outColor = vec4(pow(Lo + emissive, vec3(1.0/2.2)), 1.0f);
    outColor = vec4((ibl+ Lo + emissive), 1.0);

    // TODO: csm debugging
//    vec4 csmDebug = CSMDebugColor(viewSpacePos);
//    outColor = mix(outColor, csmDebug, 0.5f);
}