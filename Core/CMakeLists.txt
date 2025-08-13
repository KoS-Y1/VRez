add_library(MyVulkan MyVulkan/include/VulkanState.h MyVulkan/src/VulkanState.cpp MyVulkan/include/VulkanUtil.h MyVulkan/src/VulkanUtil.cpp
        MyVulkan/include/VulkanImage.h MyVulkan/src/VulkanImage.cpp MyVulkan/include/VulkanPipeline.h MyVulkan/src/VulkanPipeline.cpp
        MyVulkan/include/VulkanComputePipeline.h MyVulkan/src/VulkanComputePipeline.cpp MyVulkan/include/VulkanGraphicsPipeline.h
        MyVulkan/src/VulkanGraphicsPipeline.cpp MyVulkan/include/VulkanBuffer.h MyVulkan/src/VulkanBuffer.cpp MyVulkan/include/VulkanMesh.h
        MyVulkan/src/VulkanMesh.cpp MyVulkan/include/VertexFormats.h MyVulkan/src/VertexFormats.cpp MyVulkan/include/VulkanTexture.h MyVulkan/src/VulkanTexture.cpp)
target_include_directories(MyVulkan PUBLIC MyVulkan)
target_link_libraries(MyVulkan PUBLIC Vulkan::Vulkan Debug Util SDL3::SDL3 ShaderCompiler glm imgui MeshLoader UI Camera
        Light TextureLoader MeshInstance)

add_library(Window Window/include/Window.h Window/src/Window.cpp)
target_include_directories(Window PUBLIC Window)
target_link_libraries(Window PUBLIC SDL3::SDL3 Util Debug MyVulkan UI imgui Camera)

add_library(ShaderCompiler ShaderCompiler/include/ShaderCompiler.h ShaderCompiler/src/ShaderCompiler.cpp)
target_include_directories(ShaderCompiler PUBLIC ShaderCompiler)
target_link_libraries(ShaderCompiler PUBLIC glslang Vulkan::Vulkan SDL3::SDL3 FileSystem SPIRV-Reflect Debug)

add_library(UI UI/include/UI.h UI/src/UI.cpp)
target_include_directories(UI PUBLIC UI)
target_link_libraries(UI PUBLIC imgui Window MyVulkan glm Camera Light)

add_library(MeshLoader MeshLoader/include/MeshLoader.h MeshLoader/src/MeshLoader.cpp)
target_include_directories(MeshLoader PUBLIC MeshLoader)
target_link_libraries(MeshLoader PUBLIC MyVulkan SDL3::SDL3 tinyobjloader glm Util)

add_library(Camera Camera/include/Camera.h Camera/src/Camera.cpp)
target_include_directories(Camera PUBLIC Camera)
target_link_libraries(Camera PUBLIC glm Util MyVulkan)

add_library(Light Light/include/Light.h Light/include/LightManager.h Light/src/LightManager.cpp)
target_include_directories(Light PUBLIC Light)
target_link_libraries(Light PUBLIC glm Util MyVulkan Debug)

add_library(TextureLoader TextureLoader/include/TextureLoader.h TextureLoader/src/TextureLoader.cpp)
target_include_directories(TextureLoader PUBLIC TextureLoader)
target_link_libraries(TextureLoader PUBLIC MyVulkan Util FileSystem SDL3::SDL3 stb Debug)

add_library(MeshInstance MeshInstance/include/MeshInstance.h MeshInstance/src/MeshInstance.cpp MeshInstance/include/MeshInstanceManager.h
    MeshInstance/src/MeshInstance.cpp)
target_include_directories(MeshInstance PUBLIC MeshInstance)
target_link_libraries(MeshInstance PUBLIC MyVulkan glm Util)

add_library(ThreadPool ThreadPool/include/ThreadPool.h ThreadPool/src/ThreadPool.cpp)
target_include_directories(ThreadPool PUBLIC ThreadPool)
target_link_libraries(ThreadPool PUBLIC Util Debug)