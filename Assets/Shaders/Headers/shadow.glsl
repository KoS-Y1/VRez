#ifndef SHADOW_GLSL
#define SHADOW_GLSL

#include <util.glsl>
#include <uniform_lights.glsl>

layout (set = SHADOW_SET, binding = 0) uniform sampler2DShadow uShadowMap;

float ReadShadowMap(vec3 worldPosition){
    vec4 lightSpacePos = uLightSpaceMatrix * vec4(worldPosition, 1.0);
    vec3 ndc = lightSpacePos.xyz / lightSpacePos.w;
    vec2 uv = ndc.xy * 0.5 + 0.5;

    if(uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
    {
        return 1.0f;
    }
    
    return texture(uShadowMap, vec3(uv, ndc.z));
}

#endif