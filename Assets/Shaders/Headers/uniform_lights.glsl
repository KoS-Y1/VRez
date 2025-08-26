#ifndef UNIFORM_LIGHTS_GLSL
#define UNIFORM_LIGHTS_GLSL

#include <util.glsl>

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

layout(std140, set = UNIFORM_SET, binding = 1) uniform Lights
{
    int uLightCount;
    int uPadding0;
    int uPadding1;
    int uPadding2;
    mat4 uLightSpaceMatix;
    Light uLights[MAX_LIGHTS];
};

#endif