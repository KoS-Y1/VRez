#ifndef UNIFORM_CAMERA_GLSL
#define UNIFORM_CAMERA_GLSL

layout(set = 0, binding = 0) uniform CamaraData
{
    mat4 uView;
    mat4 uProjecton;
};

#endif