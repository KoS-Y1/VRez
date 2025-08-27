#version 450

#include <util.glsl>
#include <uniform_lights.glsl>
#include <uniform_camera.glsl>
#include <pbr.glsl>
#include <ibl.glsl>
#include <shadow.glsl>

layout (location = 0) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = TEXTURE_SET, binding = 0) uniform sampler2D uWorldPositionMetallic;
layout (set = TEXTURE_SET, binding = 1) uniform sampler2D uWorldNormalRoughness;
layout (set = TEXTURE_SET, binding = 2) uniform sampler2D uAlbedoAmbientOcclusion;
layout (set = TEXTURE_SET, binding = 3) uniform sampler2D uEmissive;

void main()
{
    const vec4 worldPositionMetallic = texture(uWorldPositionMetallic, vTexcoord);
    const vec4 worldNormalRoughness = texture(uWorldNormalRoughness, vTexcoord);
    const vec4 albedoAO = texture(uAlbedoAmbientOcclusion, vTexcoord);
    const vec3 emissive = texture(uEmissive, vTexcoord).xyz;

    const vec3 worldPosition = worldPositionMetallic.xyz;
    const vec3 worldNormal = worldNormalRoughness.xyz;

    const vec3 albedo = vec3(albedoAO.xyz);
    const float metallic = worldPositionMetallic.w;
    const float roughness = worldNormalRoughness.w;
    const float ao = albedoAO.w;

    const vec4 viewSpacePos = uView * vec4(worldPosition, 1.0f);

    const vec3 N = worldNormal;
    const vec3 V = normalize(uViewPosition - worldPosition);
    const vec3 R = reflect(-V, N);
    const float NdotV = max(dot(N, V), 0.0f);
    const vec3 F0 = mix(vec3(0.04f), albedo, metallic);

    // PBR
    vec3 Lo = vec3(0.0f);
    for(int i = 1; i < uLightCount; ++i)
    {
        Lo += CalculatePBRLight(uLights[i].position, uLights[i].direction, uLights[i].color, uLights[i].intensity,
        uLights[i].range, uLights[i].type, N, V, worldPosition, albedo, roughness, metallic);
    }

    const float shadow = ReadShadowMap(worldPosition);
    Lo += CalculatePBRLight(uLights[0].position, uLights[0].direction, uLights[0].color, uLights[0].intensity,
    uLights[0].range, uLights[0].type, N, V, worldPosition, albedo, roughness, metallic) * (1.0f - shadow);

    // IBL
    vec3 ibl = IBL(N, NdotV, R, F0, metallic, roughness, albedo, ao) * 0.2;

    // Gamma correction
    //    outColor = vec4(pow(Lo + emissive, vec3(1.0/2.2)), 1.0f);
    outColor = vec4((ibl+ Lo + emissive), 1.0);

//    outColor = CSMDebugColor(viewSpacePos);
}