#pragma once

#include <string>

#include <simdjson.h>

namespace file_system {
class JsonFile {
public:
    explicit JsonFile(const std::string &file);

    JsonFile() = delete;

    ~JsonFile() = default;

    JsonFile(const JsonFile &)            = delete;
    JsonFile(JsonFile &&)                 = delete;
    JsonFile &operator=(const JsonFile &) = delete;
    JsonFile &operator=(JsonFile &&)      = delete;

    std::string GetString(const std::string &key);
    std::string GetString(const std::string &key, const std::string &defaultValue);
    bool        GetBool(const std::string &key);
    bool        GetBool(const std::string &key, const bool defaultValue);

private:
    simdjson::dom::parser  m_parser;
    simdjson::dom::element m_doc;

    template<typename T>
    T GetField(const std::string &key, const T &defaultValue);

    template<typename T>
    T GetCriticalField(const std::string &key);

    void DebugCheckSimdJson(const simdjson::error_code error, const std::string &log);
};
} // namespace file_system