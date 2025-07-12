project(SPIRV-Reflect CXX)
add_library(SPIRV-Reflect spirv_reflect.h spirv_reflect.c spirv_reflect.cpp include/spirv/unified1/spirv.h)
target_include_directories(SPIRV-Reflect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})