#version 450

#include <uniform_lights.glsl>
#include <uniform_camera.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in mat3 vTBN;
layout (location = 2) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D uBaseTexture;
layout (set = 1, binding = 1) uniform sampler2D uNormalMap;

void main()
{
    int i = 0;

    vec3 color = texture(uBaseTexture, vTexcoord).xyz;

    vec3 lightColor = vec3(0.0f);

    vec3 tNormal = texture(uNormalMap, vTexcoord).xyz;
    vec3 normal = normalize(vTBN * tNormal);

    for (i = 0; i < uLightCount; ++i)
    {
        vec3 viewDir = normalize(uViewPosition - vWorldPosition);
        // Point
        if (uLights[i].type == 0)
        {
            lightColor += CalculatePointLight(uLights[i], normal, vWorldPosition, viewDir, 1.0f);
        }
        // Directional
        else if (uLights[i].type == 1)
        {
            lightColor += CalculateDirectionalLight(uLights[i], normal, viewDir, 1.0f);
        }
        // Ambient
        else if (uLights[i].type == 2)
        {
            lightColor += CalculateAmbientLight(uLights[i]);
        }
    }
    vec3 result = lightColor * color;
    outColor = vec4(result, 1.0f);
}