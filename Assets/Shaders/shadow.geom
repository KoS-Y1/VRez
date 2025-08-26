#version 450

#include <uniform_lights.glsl>

layout (triangles, invocations = 3) in;

layout (triangle_strip, max_vertices = 3) out;

void main()
{
    for(int i = 0; i < 3; ++i)
    {
        gl_Position = uLightSpaceMatix[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }

    EndPrimitive();
}