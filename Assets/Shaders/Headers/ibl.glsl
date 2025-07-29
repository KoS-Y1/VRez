#ifndef IBL_GLSL
#define IBL_GLSL

#include <pbr.glsl>
#include <util.glsl>

layout (set = 2, binding = 0) uniform sampler2D uBrdfLut;
layout (set = 2, binding = 0) uniform sampler2D uIrradiance;

const float MAX_REFLECTION_LOD = 6.0;



vec3 IBL(vec3 N, float NdotV, vec3 R, vec3 F0, float metallic, float roughness, vec3 albedo, float ambientOcclusion) {
{
    vec3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    vec3 irradiance = texture(uIrradiance, SampleSphericalMap(N)).rgb;
//    vec3 reflection = SampleIBLReflection(R, roughness);
    vec2 brdf = texture(uBrdfLut, vec2(NdotV, roughness)).rg;
    vec3 specular = vec3(0.0f);
//    vec3 specular = reflection * (F * brdf.x + brdf.y);
    return (kD * irradiance * albedo + specular) * ambientOcclusion;
}

#endif