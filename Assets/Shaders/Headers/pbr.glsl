#ifndef PBR_GLSL
#define PBR_GLSL

const float PI = 3.14159265359;

// PBR functions from https://learnopengl.com/PBR/Lighting
// Schlick's approximation for Fresnel
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Normal Distribution Function: GGX
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Geometry Function: Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// From Real Shading in Unreal Engine 4 by Brian Karis, Epic Games
float CalculatePointLightAttenuation(float range, float distance)
{
    float A = distance / range;
    A *= A;
    A *= A;

    A = clamp(1.0 - A, 0.0, 1.0);
    A *= A;

    float B = distance * distance + 1;
    return A / B;
}

vec3 CalculatePBRLight(vec3 lightPos, vec3 lightDir, vec3 lightColor, float lightIntensity, float lightRange, uint lightType,
    vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float roughness, float metallic)
{
    vec3 L;
    float attenuation = 1.0;

    // Point light
    if (lightType == 0)
    {
        vec3 fragToLight = lightPos - fragPos;
        float distance = length(fragToLight);
        L = normalize(fragToLight);;
        attenuation = CalculatePointLightAttenuation(lightRange, distance);
    }
    // Directional light
    else if(lightType == 1)
    {
        L = normalize(-lightDir);
    }

    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // Fresnel reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    float G = GeometrySmith(N, V, L, roughness);
    float NDF = DistributionGGX(N, H, roughness);

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 irradiance = lightColor * lightIntensity * attenuation;
    return (kD * albedo / PI + specular) * irradiance * NdotL;
}

#endif
