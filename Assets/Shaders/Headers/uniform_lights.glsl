#ifndef UNIFORM_LIGHTS_GLSL
#define UINFORM_LIGHTS_GLSL

const int MAX_LIGHTS = 16;

struct Light
{
    vec3 color;
    uint type;

    vec3 position;
    float range;

    vec3 direction;
    float innerAngle;

    float outerAngle;
    float intensity;
    float paddings[2];
};

layout(std140, set = 0, binding = 1) uniform Lights
{
    Light uLights[MAX_LIGHTS];
    int uLightCount;
    int uPaddings[3];
};


vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, float shiness)
{
    vec3 diffuse = light.color * max(dot(normal, light.direction), 0.0f);

    // Blinn-phong
    vec3 halfDir = normalize(light.direction + viewDir);
    vec3 specular = light.color * pow(max(dot(normal, halfDir), 0.0f), shiness);

    vec3 color = (diffuse + specular) * light.intensity;

    return color;
}
#endif