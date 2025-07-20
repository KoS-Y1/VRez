#version 450

//#include <uniform_lights.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

void main()
{
    int i = 0;

    vec3 color = vec3(0.0f);

//    for(i = 0; i < uLightCount; ++i)
//    {
//        vec3 viewDir = normalize(uViewPosition - vWorldPosition);
//        // Point
//        if(uLights[i].type == 0)
//        {
//
//        }
//        // Directional
//        else if(uLights[i].type == 1)
//        {
//            color += CalculateDirectionalLight(uLights[i], vWorldNormal, viewDir, 1.0f);
//        }
//        // Ambient
//        else if(uLights[i].type == 2)
//        {
//
//        }
//        // Spot
//        else if(uLights[i].type == 3)
//        {
//
//        }
//    }

    outColor = vec4(color, 1.0f);
}