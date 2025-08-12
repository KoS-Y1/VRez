#ifndef UNIFORM_CAMERA_GLSL
#define UNIFORM_CAMERA_GLSL

layout(std140, set = 0, binding = 0) uniform CameraData
{
    mat4 uView;
    mat4 uProjection;
    vec3 uViewPosition;
    float padding0;
    vec3 uSplits;
    float padding1;
};

#endif
