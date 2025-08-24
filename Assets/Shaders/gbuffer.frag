#version 450

#include <util.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in mat3 vTBN;
layout (location = 4) in vec2 vTexcoord;

layout (location = 0) out vec4 outWorldPositionMetallic;
layout (location = 1) out vec4 outWorldNormalRoughness;
layout (location = 2) out vec4 outAlbedoAmbientOcclusion;
layout (location = 3) out vec4 outEmissive;

layout (set = TEXTURE_SET, binding = 0) uniform sampler2D uAlbedo;
layout (set = TEXTURE_SET, binding = 1) uniform sampler2D uNormal;
layout (set = TEXTURE_SET, binding = 2) uniform sampler2D uORM;
layout (set = TEXTURE_SET, binding = 3) uniform sampler2D uEmissive;

void main()
{
    const vec3 albedo = texture(uAlbedo, vTexcoord).xyz;

    const vec3 tNormal = texture(uNormal, vTexcoord).xyz * 2.0 - 1.0;
    const vec3 worldNormal = normalize(vTBN * tNormal);

    const vec3 orm = texture(uORM, vTexcoord).xyz;
    const float ao = orm.r;
    const float roughness = orm.g;
    const float metallic = orm.b;

    outWorldPositionMetallic = vec4(vWorldPosition, metallic);
    outWorldNormalRoughness = vec4(worldNormal, roughness);
    outAlbedoAmbientOcclusion = vec4(albedo, ao);
    outEmissive = texture(uEmissive, vTexcoord);
}