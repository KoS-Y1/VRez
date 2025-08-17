#include "include/JsonInput.h"

#include "include/JsonFile.h"

file_system::MaterialConfig::MaterialConfig(const std::string &jsonFile) {
    JsonFile json(jsonFile);

    albedo   = json.GetString("albedo", "albedo");
    normal   = json.GetString("normal", "normal");
    orm      = json.GetString("orm", "orm");
    emissive = json.GetString("emissive", "emissive");
    pipeline = json.GetString("pipeline");
}

file_system::SkyboxConfig::SkyboxConfig(const std::string &jsonFile) {
    JsonFile json(jsonFile);

    emissive   = json.GetString("emissive");
    specular   = json.GetString("specular");
    irradiance = json.GetString("irradiance");
    brdf       = json.GetString("brdf");
}

file_system::ObjectConfig::ObjectConfig(const std::string &jsonFile) {
    JsonFile json(jsonFile);

    mesh = json.GetString("mesh");
    material = json.GetString("material");
}
