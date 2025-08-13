#pragma once

#include <string>

#include <include/VertexFormats.h>

namespace file_system {
std::vector<VertexPNTT> LoadMesh(const std::string &file);
} // namespace file_system