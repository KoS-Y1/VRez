#ifndef CSM_GLSL
#define CSM_GLSL
// TODO: descriptor layout for shadow map

#include <uniform_camera.glsl>

int GetLayer(vec4 viewSpacePos)
{
    int layer = 0;
    if(viewSpacePos.z > uSplits[0])
    {
        layer = 0;
    }
    if(viewSpacePos.z > uSplits[1])
    {
        layer = 1;
    }
    if(viewSpacePos.z > uSplits[2])
    {
        layer = 2;
    }
    if(viewSpacePos.z > uSplits[3])
    {
        layer = 3;
    }
    return layer;
}

// TODO: for testing
vec4 CSMDebugColor(vec4 viewSpacePos)
{
    int layer = GetLayer(viewSpacePos);
    vec4 color;

    if(layer == 0)
    {
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    if(layer == 1)
    {
        color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    if(layer == 2)
    {
        color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    if(layer == 3)
    {
        color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
    }

    return color;
}
#endif