add_library(Util INTERFACE Singleton.h Pointee.h)
target_include_directories(Util INTERFACE ./)

add_library(Debug INTERFACE Debug/Debug.h)
target_include_directories(Debug INTERFACE Debug)
target_link_libraries(Debug INTERFACE SDL3::SDL3 Vulkan::Vulkan)

add_library(FileSystem FileSystem/include/FileSystem.h FileSystem/src/FileSystem.cpp FileSystem/include/TextureLoader.h
        FileSystem/src/TextureLoader.cpp FileSystem/include/MeshLoader.h FileSystem/src/MeshLoader.cpp FileSystem/include/JsonFile.h
        FileSystem/src/JsonFile.cpp FileSystem/include/JsonInput.h FileSystem/src/JsonInput.cpp)
target_include_directories(FileSystem PUBLIC FileSystem)
target_link_libraries(FileSystem PUBLIC SDL3::SDL3 stb Debug MyVulkan tinyobjloader simdjson)
