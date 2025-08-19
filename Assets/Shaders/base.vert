#version 450

#include <uniform_camera.glsl>

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec2 inTexcoord;

layout (location = 0) out vec3 vWorldPositon;
layout (location = 1) out mat3 vTBN;
layout (location = 4) out vec2 vTexcoord;

layout (push_constant) uniform PushConstantData
{
    mat4 inModel;
};

void main()
{
    // Transform positon
    vWorldPositon = (inModel * vec4(inPosition, 1.0f)).xyz;
    gl_Position = uProjection * uView * vec4(vWorldPositon, 1.0f);

    // Build TBN matrix in world space
    mat3 normalMatrix = transpose(inverse(mat3(inModel)));
    vec3 T = normalize(normalMatrix * inTangent);
    vec3 N = normalize(normalMatrix * inNormal);
    vec3 B = cross(N, T);
    vTBN = mat3(T, B, N);

    vTexcoord = inTexcoord;
}
