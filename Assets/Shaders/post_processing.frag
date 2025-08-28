#version 450

#include <util.glsl>
#include <uniform_lights.glsl>
#include <uniform_camera.glsl>
#include <fxaa.glsl>

layout (location = 0) in vec2 vTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = TEXTURE_SET, binding = 0) uniform sampler2D uDrawImage;

void main()
{
    outColor = vec4(FXAA(uDrawImage, vTexcoord, uResolution), 1.0f);
}
