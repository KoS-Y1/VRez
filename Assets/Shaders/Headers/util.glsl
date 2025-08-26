#ifndef UTIL_GLSL
#define UTIL_GLSL

const int UNIFORM_SET = 0;
const int TEXTURE_SET = 1;
const int IBL_SET = 2;
const int SHADOW_SET = 3;

const float INF = 1.0 / 0.0;
const float LN2 = 0.6931471805599453;
// From https://learnopengl.com/PBR/IBL/Specular-IBL
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec4 RGBMToLinear(vec4 value)
{
    return vec4(value.rgb * value.a * 6.0f, 1.0);
}

#endif
