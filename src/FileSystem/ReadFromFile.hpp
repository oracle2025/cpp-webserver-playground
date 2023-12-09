#pragma once
#include <string>

namespace FileSystem {
class ReadFromFile {
public:
    static std::string read_string_from_file(const std::string& filename);
};
} // namespace FileSystem
