#include "include/FileSystem.h"

#include <fstream>

std::string file_system::Read(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("File could not be opened");

    size_t fileSize = file.tellg();
    std::string buf(fileSize, '\0');
    file.seekg(0, std::ios::beg);
    file.read(&buf[0], fileSize);
    file.close();

    return buf;
}
