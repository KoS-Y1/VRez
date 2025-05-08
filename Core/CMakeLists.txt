add_library(JobSystem JobSystem/Job.h JobSystem/JobSystem.h JobSystem/JobSystem.cpp)
target_include_directories(JobSystem PUBLIC JobSystem)
target_link_libraries(JobSystem PUBLIC Util)

add_library(MyVulkan MyVulkan/VulkanState.h MyVulkan/VulkanState.cpp)
target_include_directories(MyVulkan PUBLIC MyVulkan)
target_link_libraries(MyVulkan PUBLIC Vulkan::Vulkan Debug Util SDL3::SDL3)

add_library(Window Window/Window.h Window/Window.cpp)
target_include_directories(Window PUBLIC Window)
target_link_libraries(Window PUBLIC SDL3::SDL3 Util Debug MyVulkan)