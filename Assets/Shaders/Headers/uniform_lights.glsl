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
    float intensity;
};

layout(std140, set = 0, binding = 1) uniform Lights
{
    int uLightCount;
    int uPadding0;
    int uPadding1;
    int uPadding2;
    Light uLights[MAX_LIGHTS];
};


vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir, float shiness)
{
    vec3 diffuse = light.color * max(dot(normal, -light.direction), 0.0f);

    // Blinn-phong
    vec3 halfDir = normalize(-light.direction + viewDir);
    vec3 specular = light.color * pow(max(dot(normal, halfDir), 0.0f), shiness);

    return (diffuse + specular) * light.intensity;

}

vec3 CalculateAmbientLight(Light light)
{
    return light.color * light.intensity;
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, float shiness)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Attenuation
    float dist = length(light.position - fragPos);
    if (dist > light.range)
    {
        return vec3(0.0f);
    }
    float kConstant = 1.0f;
    float kLinear = 0.045f;
    float kQuadratic = 0.0075f;
    float attenuation = 1.0f / (kConstant + kLinear * dist + kQuadratic * dist * dist);

    // Diffuse
    vec3 diffuse = light.color * max(dot(normal, lightDir), 0.0f);

    // Blinn-phong specular
    float specularStrength = 0.5f;
    vec3 halfDir = normalize(lightDir + viewDir);
    vec3 specular = specularStrength * light.color * pow(max(dot(normal, halfDir), 0.0f), shiness);

    //    float attenuation = clamp(1.0 - dist / light.range, 0.0, 1.0);
    return attenuation * (diffuse + specular) * light.intensity;
}

#endif