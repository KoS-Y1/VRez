#include "include/JsonInput.h"

#include "include/JsonFile.h"

file_system::MaterialConfig::MaterialConfig(const std::string &jsonFile) {
    JsonFile json(jsonFile);

    albedo   = json.GetString("albedo", "albedo");
    normal   = json.GetString("normal", "normal");
    orm      = json.GetString("orm", "orm");
    emissive = json.GetString("emissive", "emissive");
}

file_system::ObjectConfig::ObjectConfig(const std::string &jsonFile) {
    JsonFile json(jsonFile);

    mesh = json.GetString("mesh");
    material = json.GetString("material");
}
