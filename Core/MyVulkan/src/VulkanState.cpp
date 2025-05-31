#include "include/VulkanState.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>
#include <include/VulkanUtil.h>
#include <imgui_impl_vulkan.h>
#include <glm/glm.hpp>

#include <include/VulkanComputePipeline.h>
#include <include/VulkanGraphicsPipeline.h>

VulkanState::VulkanState(SDL_Window *window, uint32_t width, uint32_t height)
{
    m_window = window;
    m_width = width;
    m_height = height;

    CreateInstance();
    CreatePhysicalDevice();
    CreateDevice();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSurface(window);
    CreateSwapchain(width, height);

    renderFence = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
    renderSemaphore = CreateSemaphore();
    presentSemaphore = CreateSemaphore();

    CreateDescriptorPool();
    CreatePipelines();

    for (auto &setLayout: pipelines[0]->GetDescriptorSetLayouts())
    {
        CreateDescriptorSet(setLayout);
    }

    UpdateDescriptorSets();
}

VulkanState::~VulkanState()
{
    WaitIdle();

    vkFreeDescriptorSets(device, descriptorPool, descriptorSets.size(), descriptorSets.data());

    for (size_t i = 0; i < pipelines.size(); ++i)
    {
        pipelines[i]->Destroy();
    }

    drawImage.Destroy();

    for (size_t i = 0; i < swapchain.count; i++)
    {
        vkDestroyImageView(device, swapchain.views[i], nullptr);
    }


    deletionQueue.flush();
}

void VulkanState::CreateInstance()
{
    VkApplicationInfo infoApp
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "VulkanRayTracerApp",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "VulkanRayTracerEngine",
        .apiVersion = VK_API_VERSION_1_4,
    };

    // Enable validation layer
    std::vector<const char *> layers
    {
        "VK_LAYER_KHRONOS_validation",
    };

    uint32_t sdlExtensionCount = 0;
    const char *const *sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    std::vector<const char *> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


    VkInstanceCreateInfo infoInstance
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &infoApp,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    DEBUG_VK_ASSERT(vkCreateInstance(&infoInstance, nullptr, &instance));

    deletionQueue.pushFunction([&]()
    {
        vkDestroyInstance(instance, nullptr);
    });
}

void VulkanState::CreatePhysicalDevice()
{
    // Query the num of physical devices
    uint32_t physicalDeviceCount = 0;
    DEBUG_VK_ASSERT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
    DEBUG_ASSERT(physicalDeviceCount > 0);
    // Query all physical devices
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    DEBUG_VK_ASSERT(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

    // Get the physical device with the max api version(usually the best one)
    uint32_t maxApiVersion = 0;
    for (VkPhysicalDevice p: physicalDevices)
    {
        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(p, &properties);

        if (properties.apiVersion > maxApiVersion)
        {
            physicalDevice = p;
            maxApiVersion = properties.apiVersion;
        }
    }

    VkPhysicalDeviceProperties properties = {0};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    SDL_Log("Selected physical device: %s %d.%d.%d", properties.deviceName,
            VK_API_VERSION_MAJOR(properties.apiVersion), VK_API_VERSION_MINOR(properties.apiVersion),
            VK_API_VERSION_PATCH(properties.apiVersion));
}

void VulkanState::CreateDevice()
{
    float priority = 1.0f;
    // Only use one queue, and use the first one
    VkDeviceQueueCreateInfo infoQueue
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = 0,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    // Enable swapchain extension for presenting on screen
    std::vector<const char *> extensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_create_renderpass2",
        "VK_KHR_depth_stencil_resolve",
        "VK_KHR_dynamic_rendering"
    };

    VkPhysicalDeviceVulkan13Features feature13
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = nullptr,
        .dynamicRendering = VK_TRUE,
    };

    VkDeviceCreateInfo infoDevice
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &feature13,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &infoQueue,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr,
    };
    DEBUG_VK_ASSERT(vkCreateDevice(physicalDevice, &infoDevice, nullptr, &device));

    // Get queue
    vkGetDeviceQueue(device, 0, 0, &queue);

    deletionQueue.pushFunction([&]()
    {
        vkDestroyDevice(device, nullptr);
    });
}

void VulkanState::CreateCommandPool()
{
    VkCommandPoolCreateInfo infoCommandPool
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };
    DEBUG_VK_ASSERT(vkCreateCommandPool(device, &infoCommandPool, nullptr, &commandPool));
}

void VulkanState::CreateSurface(SDL_Window *window)
{
    DEBUG_ASSERT(SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface));

    deletionQueue.pushFunction([&]()
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    });
}


void VulkanState::CreateSwapchain(uint32_t width, uint32_t height)
{
    VkSwapchainCreateInfoKHR infoSwapchain
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = MIN_SWAPCHAIN_IMG_COUNT,
        .imageFormat = IMG_FORMAT,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = {width, height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = false,
        .oldSwapchain = nullptr,
    };
    DEBUG_VK_ASSERT(vkCreateSwapchainKHR(device, &infoSwapchain, nullptr, &swapchain.swapchain));

    // Get swapchain images
    DEBUG_VK_ASSERT(vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.count, nullptr));
    DEBUG_VK_ASSERT(vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.count, swapchain.images));

    // Create image view for each swapchian image
    {
        VkImageViewCreateInfo infoView
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = IMG_FORMAT,
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = vk_util::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT)
        };

        for (size_t i = 0; i < swapchain.count; i++)
        {
            infoView.image = swapchain.images[i];
            DEBUG_VK_ASSERT(vkCreateImageView(device, &infoView, nullptr, &swapchain.views[i]));
        }
    }

    // Init drawImage that swapchain images copy from
    VulkanImage img(physicalDevice, device, IMG_FORMAT,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, {m_width, m_height, 1}, VK_IMAGE_ASPECT_COLOR_BIT);


    drawImage = std::move(img);

    deletionQueue.pushFunction([&]()
    {
        vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);
    });
}

VkSemaphore VulkanState::CreateSemaphore()
{
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    DEBUG_VK_ASSERT(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));

    // Pass the copy of the object since there's no such class memeber
    deletionQueue.pushFunction([this, semaphore]()
    {
        vkDestroySemaphore(device, semaphore, nullptr);
    });

    return semaphore;
}

VkFence VulkanState::CreateFence(const VkFenceCreateFlags flag)
{
    VkFence fence = VK_NULL_HANDLE;
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flag,
    };
    DEBUG_VK_ASSERT(vkCreateFence(device, &createInfo, nullptr, &fence));

    deletionQueue.pushFunction([this, fence]()
    {
        vkDestroyFence(device, fence, nullptr);
    });

    return fence;
}


void VulkanState::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo infoCmdBuffer
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    DEBUG_VK_ASSERT(vkAllocateCommandBuffers(device, &infoCmdBuffer, &cmdBuf));

    deletionQueue.pushFunction([&]()
    {
        vkDestroyCommandPool(device, commandPool, nullptr);
    });
}

void VulkanState::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize
    {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
    };

    // TODO: This is just for testing for now
    // Needs to update in the future
    VkDescriptorPoolCreateInfo infoPool
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = MAX_DESCRIPTOR_SET_COUNT,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    DEBUG_VK_ASSERT(vkCreateDescriptorPool(device, &infoPool, nullptr, &descriptorPool));

    deletionQueue.pushFunction([&]()
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    });

    // Create descriptor pool for ImGui
    VkDescriptorPoolSize poolSizes[]
    {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };

    infoPool.maxSets = 1000;
    infoPool.poolSizeCount = (uint32_t) std::size(poolSizes);
    infoPool.pPoolSizes = poolSizes;

    DEBUG_VK_ASSERT(vkCreateDescriptorPool(device, &infoPool, nullptr, &imguiDescriptorPool));
    deletionQueue.pushFunction([&]()
    {
        vkDestroyDescriptorPool(device, imguiDescriptorPool, nullptr);
    });
}

void VulkanState::CreateDescriptorSet(const VkDescriptorSetLayout layout)
{
    VkDescriptorSet set = VK_NULL_HANDLE;

    VkDescriptorSetAllocateInfo infoSet
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout
    };

    DEBUG_VK_ASSERT(vkAllocateDescriptorSets(device, &infoSet, &set));

    descriptorSets.push_back(std::move(set));
}


void VulkanState::WaitIdle()
{
    DEBUG_VK_ASSERT(vkDeviceWaitIdle(device));
}


void VulkanState::Present()
{
    // Reset fence and command buffer
    WaitAndResetFence(renderFence);
    DEBUG_VK_ASSERT(vkResetCommandBuffer(cmdBuf, 0));

    // Acquire next image in the swapchain for presenting
    uint32_t imageIndex = 0;
    DEBUG_VK_ASSERT(
        vkAcquireNextImageKHR(device, swapchain.swapchain, POINT_ONE_SECOND, presentSemaphore, nullptr, &imageIndex));

    BeginCommandBuffer(0);


    // Layout transition so that we can clear image color
    vk_util::CmdImageLayoutTransition(cmdBuf, drawImage.GetImage(), VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT, 0,
                                      VK_ACCESS_TRANSFER_WRITE_BIT);
    // Clear color image
    DrawBackground();

    // Layout transition for drawing
    vk_util::CmdImageLayoutTransition(cmdBuf, drawImage.GetImage(), VK_IMAGE_LAYOUT_GENERAL,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
                                      VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    // Draw geometry
    DrawGeometry();

    // Layout transition for copying image
    vk_util::CmdImageLayoutTransition(cmdBuf, drawImage.GetImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
                                      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                      VK_ACCESS_TRANSFER_WRITE_BIT);
    vk_util::CmdImageLayoutTransition(cmdBuf, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
                                      0, VK_ACCESS_TRANSFER_WRITE_BIT);

    // Copy draw image to the current swapchian image
    vk_util::CopyImageToImage(cmdBuf, drawImage.GetImage(), swapchain.images[imageIndex], drawImage.GetExtent(),
                              {m_width, m_height, 1}, VK_IMAGE_ASPECT_COLOR_BIT);

    // Layout transition for imgui draw
    vk_util::CmdImageLayoutTransition(cmdBuf, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
                                      VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);
    DrawImgui(swapchain.views[imageIndex]);


    // Layout transition for presenting
    vk_util::CmdImageLayoutTransition(cmdBuf, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT,
                                      VK_ACCESS_TRANSFER_WRITE_BIT, 0);

    EndAndSubmitCommandBuffer(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, renderFence, presentSemaphore, renderSemaphore);

    QueuePresent(renderSemaphore, imageIndex);
}

void VulkanState::CreatePipelines()
{
    std::vector<std::vector<std::string> > shaderPaths
    {
        {"../Assets/Shaders/gradient.comp"},
        {
            "../Assets/Shaders/colored_triangle.vert",
            "../Assets/Shaders/colored_triangle.frag"
        }
    };

    // TODO: This is just for testing, update this in the future, need better writing
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    VkDescriptorSetLayoutBinding binding
    {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .pImmutableSamplers = nullptr
    };
    bindings.push_back(binding);

    DescriptorSetLayoutConfig config
    {
        .flag = 0,
        .bindings = bindings,
    };

    std::vector<DescriptorSetLayoutConfig> configs;
    configs.push_back(config);

    std::vector<glm::vec4> constants
    {
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    };
    std::vector<VkPushConstantRange> pushConstants
    {
        {
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset = 0,
            .size = sizeof(constants)
        }
    };

    GraphicsPipelineConfig graphicsConfig;

    for (const auto &paths: shaderPaths)
    {
        // Get the shader stage from the first passed shader code
        VkShaderStageFlagBits stage = vk_util::GetStage(paths[0]);

        switch (stage)
        {
            case VK_SHADER_STAGE_VERTEX_BIT:
            case VK_SHADER_STAGE_FRAGMENT_BIT:
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            case VK_SHADER_STAGE_GEOMETRY_BIT:
                pipelines.emplace_back(std::make_unique<VulkanGraphicsPipeline>(device, paths, graphicsConfig,
                    std::vector<DescriptorSetLayoutConfig>{}, std::vector<VkPushConstantRange>{},
                    std::vector<VkFormat>{IMG_FORMAT}));
                break;

            case VK_SHADER_STAGE_COMPUTE_BIT:
                pipelines.emplace_back(std::make_unique<VulkanComputePipeline>(device, paths, configs, pushConstants));
                break;

            default:
                break;
        }
    }
}

void VulkanState::WaitAndResetFence(VkFence fence, uint64_t timeout)
{
    DEBUG_VK_ASSERT(vkWaitForFences(device, 1, &fence, true, timeout));
    DEBUG_VK_ASSERT(vkResetFences(device, 1, &fence));
}

void VulkanState::BeginCommandBuffer(VkCommandBufferUsageFlags const flag)
{
    VkCommandBufferBeginInfo infoBegin{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = flag,
        .pInheritanceInfo = nullptr,
    };
    DEBUG_VK_ASSERT(vkBeginCommandBuffer(cmdBuf, &infoBegin));
}

void VulkanState::EndAndSubmitCommandBuffer(VkPipelineStageFlags const waitStageMask, VkFence fence,
                                            VkSemaphore waitSemaphore, VkSemaphore signalSemaphore)
{
    DEBUG_VK_ASSERT(vkEndCommandBuffer(cmdBuf));

    VkSubmitInfo infoSubmit{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .pWaitSemaphores = &waitSemaphore,
        .pWaitDstStageMask = &waitStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuf,
        .pSignalSemaphores = &signalSemaphore,
    };
    if (waitSemaphore != VK_NULL_HANDLE)
    {
        infoSubmit.waitSemaphoreCount = 1;
    }
    if (signalSemaphore != VK_NULL_HANDLE)
    {
        infoSubmit.signalSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueueSubmit(queue, 1, &infoSubmit, fence));
}

void VulkanState::QueuePresent(VkSemaphore waitSemaphore, uint32_t imageIndex)
{
    VkPresentInfoKHR infoPresent{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .pWaitSemaphores = &waitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain.swapchain,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };
    if (waitSemaphore != VK_NULL_HANDLE)
    {
        infoPresent.waitSemaphoreCount = 1;
    }
    DEBUG_VK_ASSERT(vkQueuePresentKHR(queue, &infoPresent));
}

void VulkanState::DrawBackground()
{
    // Compute pipeline to dispatch
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines[0]->GetPipeline());
    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines[0]->GetLayout(), 0, 1, &descriptorSets[0],
                            0,
                            nullptr);

    std::vector<glm::vec4> constants
    {
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    };
    vkCmdPushConstants(cmdBuf, pipelines[0]->GetLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(constants),
                       constants.data());
    vkCmdDispatch(cmdBuf, std::ceil(m_width / 16.0), std::ceil(m_height / 16.0), 1);
}

// TODO: This should not belong here
void VulkanState::UpdateDescriptorSets()
{
    VkDescriptorImageInfo infoImage
    {
        .sampler = VK_NULL_HANDLE,
        .imageView = drawImage.GetImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    VkWriteDescriptorSet writeSet
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = descriptorSets[0],
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &infoImage,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(device, 1, &writeSet, 0, nullptr);
}

void VulkanState::DrawImgui(VkImageView view)
{
    VkRect2D renderAreas
    {
        .offset = {0, 0},
        .extent = {m_width, m_height}
    };
    VkRenderingAttachmentInfo infoColorAttachment = vk_util::GetRenderingAttachmentInfo(
        view, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, nullptr);
    VkRenderingInfo infoRendering = vk_util::GetRenderingInfo(renderAreas, &infoColorAttachment);

    vkCmdBeginRendering(cmdBuf, &infoRendering);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

    vkCmdEndRendering(cmdBuf);
}

void VulkanState::DrawGeometry()
{
    VkRect2D renderAreas
    {
        .offset = {0, 0},
        .extent = {m_width, m_height}
    };
    VkRenderingAttachmentInfo infoAttachment = vk_util::GetRenderingAttachmentInfo(
        drawImage.GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, nullptr);
    VkRenderingInfo infoRender = vk_util::GetRenderingInfo(renderAreas, &infoAttachment);

    vkCmdBeginRendering(cmdBuf, &infoRender);

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[1]->GetPipeline());

    // Set dynamic viewport and scissor
    VkViewport viewport
    {
        .x = 0.f,
        .y = 0.f,
        .width = (float) m_width,
        .height = (float) m_height,
        .minDepth = 0.f,
        .maxDepth = 1.f
    };

    vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
    vkCmdSetScissor(cmdBuf, 0, 1, &renderAreas);

    vkCmdDraw(cmdBuf, 3, 1, 0, 0);

    vkCmdEndRendering(cmdBuf);
}
