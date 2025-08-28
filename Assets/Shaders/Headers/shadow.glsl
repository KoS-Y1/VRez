#ifndef SHADOW_GLSL
#define SHADOW_GLSL

#include <util.glsl>
#include <uniform_lights.glsl>



layout (set = SHADOW_SET, binding = 0) uniform sampler2DShadow uShadowMap;

float CalculateBias(vec3 worldNormal, vec3 lightDir){
    vec3 L = -lightDir;
    float NdotL = clamp(dot(normalize(worldNormal), normalize(lightDir)), 0.0f, 1.0f);
    float slope = 1.0f - NdotL;

    const vec2 texel = textureSize(uShadowMap, 0);
    const float texelSize = 1.0f / min(texel.x, texel.y);
    const float bias = 0.05f;
    const float slopeFactor = 4.0f;

    return (bias + slopeFactor * slope) * texelSize;
}

float CalculatePCF(vec2 uv, float cmp, vec3 worldNormal)
{
    float shadow = 0.0f;
    const float bias = CalculateBias(worldNormal, uLights[0].direction);
    const vec2 texelSize = 1.0f / textureSize(uShadowMap, 0);

    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            shadow += texture(uShadowMap, vec3(uv + vec2(x, y) * texelSize, cmp - bias));
        }

    }
    shadow /= 9.0f;
    return shadow;
}

float ReadShadowMap(vec3 worldPosition, vec3 worldNormal){
    vec4 lightSpacePos = uLightSpaceMatrix * vec4(worldPosition, 1.0);
    vec3 ndc = lightSpacePos.xyz / lightSpacePos.w;
    vec2 uv = ndc.xy * 0.5 + 0.5;
    uv = vec2(uv.x, uv.y);

    if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
    {
        return 1.0f;
    }

    return CalculatePCF(uv, ndc.z, worldNormal);
}

#endif