#ifndef UNIFORM_CAMERA_GLSL
#define UNIFORM_CAMERA_GLSL

#include <util.glsl>

layout(std140, set = UNIFORM_SET, binding = 0) uniform CameraData
{
    mat4 uView;
    mat4 uProjection;
    vec3 uViewPosition;
    float padding0;
};

#endif
