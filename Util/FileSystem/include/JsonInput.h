#pragma once
#include <string>

// TODO: adjust it for deferred rendering
struct MaterialConfig {
    std::string albedo;
    std::string normal;
    std::string orm;
    std::string emissive;
};

struct SkyboxConfig {
    std::string emissive;
    std::string specular;
    std::string irradiance;
    std::string brdf;
};

struct ObjectConfig {
    std::string material;
    std::string mesh;
};