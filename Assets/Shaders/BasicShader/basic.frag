#version 450

#include <uniform_lights.glsl>
#include <uniform_camera.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D uBaseTexture;

void main()
{
    int i = 0;

    vec3 color = texture(uBaseTexture, vTexcoord).xyz;

    vec3 lightColor = vec3(0.0f);

    for (i = 0; i < uLightCount; ++i)
    {
        vec3 viewDir = normalize(uViewPosition - vWorldPosition);
        // Point
        if (uLights[i].type == 0)
        {
            lightColor += CalculatePointLight(uLights[i], vWorldNormal, vWorldPosition, viewDir, 1.0f);
        }
        // Directional
        else if (uLights[i].type == 1)
        {
            lightColor += CalculateDirectionalLight(uLights[i], vWorldNormal, viewDir, 1.0f);
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