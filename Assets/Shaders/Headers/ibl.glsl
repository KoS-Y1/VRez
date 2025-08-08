#ifndef IBL_GLSL
#define IBL_GLSL

#include <pbr.glsl>
#include <util.glsl>

layout (set = 2, binding = 0) uniform sampler2D uBrdfLut;
layout (set = 2, binding = 1) uniform sampler2D uIrradiance;
layout (set = 2, binding = 2) uniform sampler2D uSpecular;

const float MAX_REFLECTION_LOD = 6.0;

// ibl textures generated using https://github.com/oframe/ibl-converter
vec3 SampleIBLReflection(vec3 R, float roughness) {
    // based on https://github.com/oframe/ibl-converter/blob/master/src/shaders/PBRShader.js
    float blend = roughness * MAX_REFLECTION_LOD;
    float level0 = floor(blend);
    float level1 = min(MAX_REFLECTION_LOD, level0 + 1.0);
    blend -= level0;

    vec2 uvSpec = SampleSphericalMap(R);
    uvSpec.y /= 2.0;

    vec2 uv0 = uvSpec;
    vec2 uv1 = uvSpec;

    uv0 /= pow(2.0, level0);
    uv0.y += 1.0 - exp(-LN2 * level0);

    uv1 /= pow(2.0, level1);
    uv1.y += 1.0 - exp(-LN2 * level1);

    vec3 specular0 = RGBMToLinear(texture(uSpecular, uv0)).rgb;
    vec3 specular1 = RGBMToLinear(texture(uSpecular, uv1)).rgb;
    vec3 reflection = mix(specular0, specular1, blend);

    return reflection;
}

vec3 IBL(vec3 N, float NdotV, vec3 R, vec3 F0, float metallic, float roughness, vec3 albedo, float ambientOcclusion)
{
    vec3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    vec3 irradiance = texture(uIrradiance, SampleSphericalMap(N)).rgb;
    vec3 reflection = SampleIBLReflection(R, roughness);
    vec2 brdf = texture(uBrdfLut, vec2(NdotV, roughness)).rg;
    vec3 specular = reflection * (F * brdf.x + brdf.y);
    return (kD * irradiance * albedo + specular) * ambientOcclusion;
}

#endif
