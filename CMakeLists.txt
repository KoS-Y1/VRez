cmake_minimum_required(VERSION 3.26)
project(VulkanRayTracer)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED )

add_definitions(-D GLM_ENABLE_EXPERIMENTAL)

add_subdirectory(Core)
add_subdirectory(Util)
add_subdirectory(External)

add_executable(VulkanRayTracer main.cpp)

target_link_libraries(VulkanRayTracer Vulkan::Vulkan SDL3::SDL3 Debug Window glslang Resource)