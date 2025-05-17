add_library(JobSystem JobSystem/include/Job.h JobSystem/include/JobSystem.h JobSystem/src/JobSystem.cpp)
target_include_directories(JobSystem PUBLIC JobSystem)
target_link_libraries(JobSystem PUBLIC Util)

add_library(MyVulkan MyVulkan/include/VulkanState.h MyVulkan/src/VulkanState.cpp MyVulkan/include/VulkanUtil.h MyVulkan/src/VulkanUtil.cpp
        MyVulkan/include/VulkanImage.h MyVulkan/src/VulkanImage.cpp MyVulkan/include/VulkanPipeline.h MyVulkan/src/VulkanPipeline.cpp)
target_include_directories(MyVulkan PUBLIC MyVulkan)
target_link_libraries(MyVulkan PUBLIC Vulkan::Vulkan Debug Util SDL3::SDL3)

add_library(Window Window/include/Window.h Window/src/Window.cpp)
target_include_directories(Window PUBLIC Window)
target_link_libraries(Window PUBLIC SDL3::SDL3 Util Debug MyVulkan)

add_library(ShaderCompiler ShaderCompiler/include/ShaderCompiler.h ShaderCompiler/src/ShaderCompiler.cpp)
target_include_directories(ShaderCompiler PUBLIC ShaderCompiler)
target_link_libraries(ShaderCompiler PUBLIC glslang Vulkan::Vulkan SDL3::SDL3 FileSystem)