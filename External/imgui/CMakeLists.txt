add_library(imgui imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_impl_sdl3.cpp imgui_impl_sdlgpu3.cpp imgui_impl_vulkan.cpp imgui_tables.cpp imgui_widgets.cpp)
target_include_directories(imgui INTERFACE .)
target_link_libraries(imgui PRIVATE SDL3::SDL3 Vulkan::Vulkan)
