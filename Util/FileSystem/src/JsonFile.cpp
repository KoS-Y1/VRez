#include "include/JsonFile.h"

#include <Debug.h>

#include <include/FileSystem.h>

file_system::JsonFile::JsonFile(const std::string &file) {
    const simdjson::error_code error = m_parser.parse(file_system::Read(file)).get(m_doc);

    DebugCheckSimdJson(error, ("Failed to load " + file));
}

template<typename T>
T file_system::JsonFile::GetCriticalField(const std::string &key) {
    T                          value{};
    const simdjson::error_code error = m_doc.at_key(key).get(value);
    DebugCheckSimdJson(error, ("Failed to get critical field " + key));
    return value;
}

template<typename T>
T file_system::JsonFile::GetField(const std::string &key, const T &defaultValue) {
    T                          value{};
    const simdjson::error_code error = m_doc.at_key(key).get(value);
    return error == simdjson::SUCCESS ? value : defaultValue;
}

void file_system::JsonFile::DebugCheckSimdJson(const simdjson::error_code error, const std::string &log) {
    DEBUG_ASSERT_LOG(error == simdjson::SUCCESS, log.c_str());
}

std::string file_system::JsonFile::GetString(const std::string &key) {
    return std::string(GetCriticalField<std::string_view>(key));
}

std::string file_system::JsonFile::GetString(const std::string &key, const std::string &defaultValue) {
    return std::string(GetField<std::string_view>(key, defaultValue));
}

bool file_system::JsonFile::GetBool(const std::string &key) {
    return static_cast<bool>(GetCriticalField<bool>(key));
}

bool file_system::JsonFile::GetBool(const std::string &key, const bool defaultValue) {
    return static_cast<bool>(GetField<bool>(key, defaultValue));
}