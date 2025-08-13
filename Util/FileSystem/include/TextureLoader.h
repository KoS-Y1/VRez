#pragma once

#include <string>

namespace file_system {
unsigned char *LoadTexture(const std::string &file, int *width, int *height);
}