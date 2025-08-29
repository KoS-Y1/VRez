cmake_minimum_required(VERSION 3.26)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(Core)
add_subdirectory(Util)
add_subdirectory(External)

add_library(glm_config INTERFACE)
target_compile_definitions(glm_config INTERFACE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(Camera PUBLIC glm_config)
target_link_libraries(Light PUBLIC glm_config)
target_link_libraries(UI PUBLIC glm_config)
target_link_libraries(GFX PUBLIC glm_config)
target_link_libraries(Resource PUBLIC glm_config)

add_executable(VulkanApp main.cpp)

target_link_libraries(VulkanApp Vulkan::Vulkan SDL3::SDL3 Debug Window glslang Resource)
