//  Based on NVIDIA FXAA by TIMOTHY LOTTES
#ifndef FXAA_GLSL
#define FXAA_GLSL

#define FXAA_SPAN_MAX         8.0
#endif
#ifndef FXAA_REDUCE_MUL
#define FXAA_REDUCE_MUL       (1.0/8.0)
#endif
#ifndef FXAA_REDUCE_MIN
#define FXAA_REDUCE_MIN       (1.0/128.0)
#endif
#ifndef FXAA_EDGE_THRESHOLD
#define FXAA_EDGE_THRESHOLD   (1.0/8.0)
#endif
#ifndef FXAA_EDGE_THRESHOLD_MIN
#define FXAA_EDGE_THRESHOLD_MIN (1.0/24.0)

float Luma(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

vec3 FXAA(sampler2D tex, vec2 uv, vec2 resolution)
{
    vec2 invRes = 1.0 / resolution.xy;
    vec2 v_rgbM = uv;
    vec2 v_rgbNW = (uv + vec2(-1.0f, -1.0f)) * invRes;
    vec2 v_rgbNE = (uv + vec2(1.0f, -1.0f)) * invRes;
    vec2 v_rgbSW = (uv + vec2(-1.0f, 1.0f)) * invRes;
    vec2 v_rgbSE = (uv + vec2(1.0f, 1.0f)) * invRes;

    vec3 rgbM = texture(tex, v_rgbM).rgb;
    vec3 rgbNW = texture(tex, v_rgbNW).rgb;
    vec3 rgbNE = texture(tex, v_rgbNE).rgb;
    vec3 rgbSW = texture(tex, v_rgbSW).rgb;
    vec3 rgbSE = texture(tex, v_rgbSE).rgb;

    float lumaM = Luma(rgbM);
    float lumaNW = Luma(rgbNW);
    float lumaNE = Luma(rgbNE);
    float lumaSW = Luma(rgbSW);
    float lumaSE = Luma(rgbSE);

    float lumaL = (lumaNW + lumaNE + lumaSW + lumaSE) / 4.0f;

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaRange = lumaMax - lumaMin;

    // Early exit: no significant edge
    if (lumaRange < max(FXAA_EDGE_THRESHOLD, lumaMax * FXAA_EDGE_THRESHOLD_MIN))
        return rgbM;

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
    FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = clamp(dir * rcpDirMin, vec2(-FXAA_SPAN_MAX), vec2(FXAA_SPAN_MAX)) * invRes;

    // Two-tap blend A
    vec3 rgbA = 0.5 * (
    texture(tex, uv + dir * (1.0/3.0 - 0.5)).rgb +
    texture(tex, uv + dir * (2.0/3.0 - 0.5)).rgb
    );

    // Four-tap blend B
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
    texture(tex, uv + dir * -0.5).rgb +
    texture(tex, uv + dir *  0.5).rgb
    );

    // Choose blend that keeps luma inside local min/max to avoid halos
    float lumaB = Luma(rgbB);
    if (lumaB < lumaMin || lumaB > lumaMax)
        return rgbA;

    return rgbB;
}

#endif