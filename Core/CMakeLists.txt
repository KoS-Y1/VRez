add_library(MyVulkan MyVulkan/include/VulkanState.h MyVulkan/src/VulkanState.cpp MyVulkan/include/VulkanUtil.h MyVulkan/src/VulkanUtil.cpp
        MyVulkan/include/VulkanImage.h MyVulkan/src/VulkanImage.cpp MyVulkan/include/VulkanPipeline.h MyVulkan/src/VulkanPipeline.cpp
        MyVulkan/include/VulkanComputePipeline.h MyVulkan/src/VulkanComputePipeline.cpp MyVulkan/include/VulkanGraphicsPipeline.h
        MyVulkan/src/VulkanGraphicsPipeline.cpp MyVulkan/include/VulkanBuffer.h MyVulkan/src/VulkanBuffer.cpp MyVulkan/include/VertexFormats.h
        MyVulkan/src/VertexFormats.cpp MyVulkan/include/Descriptor.h)
target_include_directories(MyVulkan PUBLIC MyVulkan)
target_link_libraries(MyVulkan PUBLIC Vulkan::Vulkan Debug Util SDL3::SDL3 ShaderCompiler glm imgui Window)

add_library(Window Window/include/Window.h Window/src/Window.cpp)
target_include_directories(Window PUBLIC Window)
target_link_libraries(Window PUBLIC SDL3::SDL3 Util Debug MyVulkan UI imgui Camera GFX Light)

add_library(ShaderCompiler ShaderCompiler/include/ShaderCompiler.h ShaderCompiler/src/ShaderCompiler.cpp)
target_include_directories(ShaderCompiler PUBLIC ShaderCompiler)
target_link_libraries(ShaderCompiler PUBLIC glslang Vulkan::Vulkan SDL3::SDL3 FileSystem SPIRV-Reflect Debug)

add_library(UI UI/include/UI.h UI/src/UI.cpp UI/include/UIRenderer.h UI/src/UIRenderer.cpp)
target_include_directories(UI PUBLIC UI)
target_link_libraries(UI PUBLIC imgui MyVulkan glm Camera Light Vulkan::Vulkan)

add_library(Camera Camera/include/Camera.h Camera/src/Camera.cpp)
target_include_directories(Camera PUBLIC Camera)
target_link_libraries(Camera PUBLIC glm Util)

add_library(Light Light/include/Light.h Light/include/LightManager.h Light/src/LightManager.cpp)
target_include_directories(Light PUBLIC Light)
target_link_libraries(Light PUBLIC glm Util MyVulkan Debug)
add_library(ThreadPool ThreadPool/include/ThreadPool.h ThreadPool/src/ThreadPool.cpp)
target_include_directories(ThreadPool PUBLIC ThreadPool)
target_link_libraries(ThreadPool PUBLIC Util Debug)

add_library(Resource Resource/include/ResourceManager.h Resource/include/TextureManager.h
        Resource/src/TextureManager.cpp Resource/include/MeshManager.h Resource/src/MeshManager.cpp
        Resource/include/VulkanTexture.h Resource/src/VulkanTexture.cpp  Resource/include/VulkanMesh.h
        Resource/src/VulkanMesh.cpp Resource/include/VulkanPrefab.h Resource/src/VulkanPrefab.cpp
        Resource/include/VulkanObject.h Resource/src/VulkanObject.cpp Resource/include/VulkanMaterial.h
        Resource/src/VulkanMaterial.cpp Resource/include/PipelineManager.h Resource/src/PipelineManager.cpp
        Resource/include/MaterialRegistry.h Resource/src/MaterialRegistry.cpp Resource/include/ObjectRegistry.h
        Resource/src/ObjectRegistry.cpp)
target_include_directories(Resource PUBLIC Resource)
target_link_libraries(Resource PUBLIC ThreadPool FileSystem MyVulkan Util SDL3::SDL3 glm)

add_library(GFX GFX/include/PbrRenderer.h GFX/src/PbrRenderer.cpp GFX/include/GBufferPass.h  GFX/src/GBufferPass.cpp
        GFX/include/LightingPass.h GFX/src/LightingPass.cpp GFX/include/SkyboxPass.h GFX/src/SkyboxPass.cpp
        GFX/include/RenderPass.h)
target_include_directories(GFX PUBLIC GFX)
target_link_libraries(GFX PUBLIC MyVulkan Resource Camera Light imgui)