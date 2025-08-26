#ifndef CSM_GLSL
#define CSM_GLSL

#include <util.glsl>
#include <uniform_camera.glsl>

layout (set = CSM_SET, binding = 0) uniform sampler2DArrayShadow uShadowMap;

int GetLayer(vec4 viewSpacePos)
{
    const float depth = -viewSpacePos.z;
    int layer = 0;
    if (depth > uSplits.x)
    {
        layer = 1;
    }
    if (depth > uSplits.y)
    {
        layer = 2;
    }
    if (depth > uSplits.z)
    {
        layer = 3;
    }

    return layer;
}

float PCF(vec3 shadowCoord, int layer){
    float shadow = 0.0f;
    const vec2 texelSize = 1.0f / textureSize(uShadowMap, 0).xy;

    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            vec4 coord = vec4(shadowCoord.xy + vec2(x, y) * texelSize, layer, shadowCoord.z);
            shadow += texture(uShadowMap, coord);
        }
    }
    shadow /= 25.0;

    return shadow;
}

float ReadShadowMap(vec4 viewSpacePos, vec4 worldPos)
{
    int layer = GetLayer(viewSpacePos);
    vec4 shadowCoord = uLightSpaceMatix[layer] * worldPos;
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
    shadowCoord /= shadowCoord.w;
    return shadowCoord.z <= -1.0 || shadowCoord.z >= 1.0 ? 0.0 : PCF(shadowCoord.xyz, layer);
}

#endif