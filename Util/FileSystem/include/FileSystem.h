#include <string>
#include <vector>

namespace file_system {
std::string Read(const std::string &path);
std::string GetFileName(const std::string &path);
std::vector<std::string> GetFilesWithExtension(const std::string &rootDir, const std::string &suffix);
} // namespace file_system
