add_library(Util INTERFACE Singleton.h)
target_include_directories(Util INTERFACE ./)

add_library(Debug INTERFACE Debug/Assert.h)
target_include_directories(Debug INTERFACE Debug)
target_link_libraries(Debug INTERFACE SDL3::SDL3 Vulkan::Vulkan)