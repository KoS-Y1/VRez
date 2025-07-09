#ifndef UNIFORM_VIEW_GLSL
#define UNIFORM_VIEW_GLSL

layout (set = 0, binding = 0) uniform UniformViewData
{
    mat4 uView;
    mat4 uProjecton;
};

#endif