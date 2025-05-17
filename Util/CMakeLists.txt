add_library(Util INTERFACE Singleton.h)
target_include_directories(Util INTERFACE ./)

add_library(Debug INTERFACE Debug/Assert.h)
target_include_directories(Debug INTERFACE Debug)
target_link_libraries(Debug INTERFACE SDL3::SDL3 Vulkan::Vulkan)

add_library(FileSystem FileSystem/include/FileSystem.h FileSystem/src/FileSystem.cpp)
target_include_directories(FileSystem PUBLIC FileSystem)
