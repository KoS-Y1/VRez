#pragma once
#include <string>

namespace file_system {
struct MaterialConfig {
    std::string albedo;
    std::string normal;
    std::string orm;
    std::string emissive;

    explicit MaterialConfig(const std::string &jsonFile);
};

struct SkyboxConfig {
    std::string emissive;
    std::string specular;
    std::string irradiance;
    std::string brdf;

    explicit SkyboxConfig(const std::string &jsonFile);
};

struct ObjectConfig {
    std::string mesh;
    std::string material;

    explicit ObjectConfig(const std::string &jsonFile);
};


} // namespace file_system
