#ifndef UNIFORM_LIGHTS_GLSL
#define UNIFORM_LIGHTS_GLSL

const int MAX_LIGHTS = 16;

struct Light
{
    vec3 color;
    uint type;

    vec3 position;
    float range;

    vec3 direction;
    float intensity;
};

layout(std140, set = 0, binding = 1) uniform Lights
{
    int uLightCount;
    int uPadding0;
    int uPadding1;
    int uPadding2;
    Light uLights[MAX_LIGHTS];
};

#endif