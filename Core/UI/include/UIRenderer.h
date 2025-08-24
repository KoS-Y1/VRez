#pragma once

#include <deque>
#include <functional>

#include <vulkan/vulkan.h>

#include <Singleton.h>

#include "UI.h"

class VulkanPrefab;

class UIRenderer : public Singleton<UIRenderer>{
public:
    void Init();

    void Render();

    void Present();

    void AddPrefabWindow(VulkanPrefab& prefab, size_t prefabIndex);

    void Destroy();

protected:
    UIRenderer() = default;

    ~UIRenderer() = default;

private:
    UI                                m_ui;
    std::deque<std::function<void()>> m_uiQueue;

    std::vector<bool> m_uniformScales;

    VkDescriptorPool m_imguiDescriptorPool;

    void Enqueue(std::function<void()> &&func) { m_uiQueue.push_back(func); }

    void CreateDescriptorPool();
};