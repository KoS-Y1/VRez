#include "include/UI.h"

#include "imgui.h"

#include <include/VulkanPrefab.h>

#include <include/Window.h>
#include <include/Camera.h>
#include <include/LightManager.h>

#include <glm/gtc/type_ptr.hpp>


void UI::TransformationWindow(VulkanPrefab &instance, bool &uniformScale, size_t id) {
    float step = 0.1f;

    float angleStep = 1.0f;
    float minRoll   = -180.0f;
    float maxRoll   = 180.0f;
    float minPitch  = -90.0f;
    float maxPitch  = 90.0f;

    bool isReset = false;

    glm::vec3 location     = instance.GetLocation();
    glm::vec3 scale        = instance.GetScale();
    glm::vec3 pitchYawRoll = instance.GetPitchYawRoll();
    float     yaw          = glm::degrees(pitchYawRoll.y);
    float     pitch        = glm::degrees(pitchYawRoll.x);
    float     roll         = glm::degrees(pitchYawRoll.z);


    ImGui::Begin((instance.GetName() + " Transformation Mesh#" + std::to_string(id)).c_str());
    ImGui::PushID(id);
    ImGui::DragFloat3("Location(x, y, z)", glm::value_ptr(location), step);

    ImGui::Text("Rotation");
    ImGui::PushItemWidth(50);
    ImGui::DragFloat("Yaw", &yaw, angleStep);
    ImGui::SameLine();
    ImGui::DragFloat("Pitch", &pitch, angleStep, minPitch, maxPitch);
    ImGui::SameLine();
    ImGui::DragFloat("Roll", &roll, angleStep, minRoll, maxRoll);
    ImGui::PopItemWidth();

    ImGui::DragFloat3("Scale(x, y, z)", glm::value_ptr(scale), step);
    ImGui::Checkbox("Uniform Scale", &uniformScale);
    if (ImGui::Button("Reset")) {
        isReset = true;
    }

    ImGui::PopID();
    ImGui::End();

    instance.SetLocation(location);

    instance.SetRotation(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));

    if (uniformScale) {
        if (scale.x != instance.GetScale().x) {
            scale = glm::vec3(scale.x, scale.x, scale.x);
        } else if (scale.y != instance.GetScale().y) {
            scale = glm::vec3(scale.y, scale.y, scale.y);
        } else if (scale.z != instance.GetScale().z) {
            scale = glm::vec3(scale.z, scale.z, scale.z);
        }
    }
    instance.SetScale(scale);

    if (isReset) {
        instance.Reset();
    }
}

void UI::CameraWindow() {
    float step = 0.1f;

    float minFov = 1.0f;
    float maxFov = 45.0f;

    float angleStep = 1.0f;
    float minPitch  = -90.0f;
    float maxPitch  = 90.0f;

    bool isReset = false;

    glm::vec3 location     = Camera::GetInstance().GetLocation();
    float     fov          = Camera::GetInstance().GetFOV();
    glm::vec3 pitchYawRoll = Camera::GetInstance().GetPitchYawRoll();
    float     yaw          = glm::degrees(pitchYawRoll.y);
    float     pitch        = glm::degrees(pitchYawRoll.x);

    ImGui::Begin("Camera(Press LSHIFT to switch to camera mode");
    ImGui::DragFloat3("Location(x, y, z)", glm::value_ptr(location), step);
    ImGui::DragFloat("FOV", &fov, step, minFov, maxFov);

    ImGui::Text("Rotation");
    ImGui::PushItemWidth(50);
    ImGui::DragFloat("Yaw", &yaw, angleStep);
    ImGui::SameLine();
    ImGui::DragFloat("Pitch", &pitch, angleStep, minPitch, maxPitch);
    ImGui::PopItemWidth();

    if (ImGui::Button("Reset")) {
        isReset = true;
    }

    ImGui::End();

    Camera::GetInstance().SetLocation(location);
    Camera::GetInstance().SetFov(fov);
    Camera::GetInstance().SetRotation(glm::vec3(glm::radians(pitch), glm::radians(yaw), pitchYawRoll.z));

    if (isReset) {
        Camera::GetInstance().Reset();
    }
}

void UI::LightsWindow() {
    bool addPoint       = false;
    bool addDirectional = false;

    int removeLightIndex = -1;

    ImGui::Begin("Lights(Max 16)");
    if (ImGui::Button("Add Point")) {
        addPoint = true;
    }
    if (ImGui::Button("Add Directional")) {
        addDirectional = true;
    }

    size_t lightCount = LightManager::GetInstance().GetLightCount();

    if (lightCount > 0) {
        for (size_t i = 0; i < lightCount; i++) {
            ImGui::PushID(i);
            std::string headerStr = "Light[" + std::to_string(i) + "]";
            if (ImGui::CollapsingHeader(headerStr.c_str())) {
                if (removeLightIndex == -1) {
                    removeLightIndex = LightSection(i);
                }
            }
            ImGui::PopID();
        }
    }

    ImGui::End();


    if (addPoint) {
        LightManager::GetInstance().AddLight(LightType::Point);
    }
    if (addDirectional) {
        LightManager::GetInstance().AddLight(LightType::Directional);
    }
    if (removeLightIndex > -1) {
        LightManager::GetInstance().RemoveLight(removeLightIndex);
    }
}

int32_t UI::LightSection(size_t idx) {
    Light    &light = const_cast<Light &>(LightManager::GetInstance().GetLight(idx));
    LightType type  = static_cast<LightType>(light.type);

    float step         = 0.01f;
    float minIntensity = 0.0f;
    float maxIntensity = 50.0f;
    float minRange     = 0.0f;
    float maxRange     = 20.f;

    bool remove = false;

    // TODO: use variables
    switch (type) {
    case LightType::Point:
        ImGui::Text("Point Light");
        ImGui::DragFloat3("Position", glm::value_ptr(light.position), step);
        ImGui::DragFloat("Range", &light.range, step, minRange, maxRange);
        break;

    case LightType::Directional:
        ImGui::Text("Directional Light");
        ImGui::DragFloat3("Direction", glm::value_ptr(light.direction), step);
        break;
    }


    ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    ImGui::DragFloat("Intensity", &light.intensity, step, minIntensity, maxIntensity);
    if (ImGui::Button("Remove")) {
        remove = true;
    }

    LightManager::GetInstance().UpdateLight(idx, light);

    if (remove) {
        return idx;
    }

    return -1;
}
