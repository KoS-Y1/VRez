#version 450

layout (location = 0) in vec3 vWorldPosition;

layout (set = 1, binding = 0) uniform sampler2D sphereMap;

layout (location = 0) out vec4 outColor;

// From https://learnopengl.com/PBR/IBL/Specular-IBL
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(vWorldPosition);

    outColor = vec4(texture(sphereMap, uv).xyz, 1.0f);
}