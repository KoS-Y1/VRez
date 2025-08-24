#pragma once

#include <deque>
#include <functional>

#include <vulkan/vulkan.h>

#include "UI.h"

class VulkanPrefab;

class UIRenderer{
public:
    UIRenderer();
    ~UIRenderer();

    UIRenderer(const UIRenderer&) = delete;
    UIRenderer(UIRenderer&&) = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;
    UIRenderer& operator=(UIRenderer&&) = delete;

    void Render();

    void Present();

    void AddPrefabWindow(VulkanPrefab& prefab, size_t prefabIndex);

private:
    UI                                m_ui;
    std::deque<std::function<void()>> m_uiQueue;

    std::vector<bool> m_uniformScales;

    VkDescriptorPool m_imguiDescriptorPool;

    void Enqueue(std::function<void()> &&func) { m_uiQueue.push_back(func); }

    void CreateDescriptorPool();
};