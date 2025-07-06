#include "include/FileSystem.h"

#include <fstream>

#include <SDL3/SDL_log.h>

std::string file_system::Read(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        SDL_Log("Failed to open file %s", path.c_str());
        exit(EXIT_FAILURE);
 }

    size_t fileSize = file.tellg();
    std::string buf(fileSize, '\0');
    file.seekg(0, std::ios::beg);
    file.read(&buf[0], fileSize);
    file.close();

    return buf;
}
