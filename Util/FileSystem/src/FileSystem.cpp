#include "include/FileSystem.h"

#include <fstream>
#include <filesystem>
#include <SDL3/SDL_log.h>

std::string file_system::Read(const std::string &path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        SDL_Log("Failed to open file %s", path.c_str());
        exit(EXIT_FAILURE);
    }

    size_t      fileSize = file.tellg();
    std::string buf(fileSize, '\0');
    file.seekg(0, std::ios::beg);
    file.read(&buf[0], fileSize);
    file.close();

    return buf;
}

std::string file_system::GetFileName(const std::string &path) {
    size_t pos = path.find_last_of("/\\");
    // If no slash found
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

std::vector<std::string> file_system::GetFilesWithExtension(const std::string &rootDir, const std::string &suffix) {
    std::vector<std::string> files;

    std::filesystem::path root(rootDir);
    if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
        return files;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && entry.path().extension() == suffix) {
            std::string path = entry.path().string();
            std::replace(path.begin(), path.end(), '\\', '/');
            files.push_back(path);
        }
    }

    return files;
}
