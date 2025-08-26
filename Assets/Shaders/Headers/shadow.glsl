#ifndef SHADOW_GLSL
#define SHADOW_GLSL

#include <util.glsl>
#include <uniform_camera.glsl>

layout (set = SHADOW_SET, binding = 0) uniform sampler2DShadow uShadowMap;

#endif