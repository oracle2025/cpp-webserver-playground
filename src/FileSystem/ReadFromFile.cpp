#include "ReadFromFile.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
namespace FileSystem {

std::string ReadFromFile::read_string_from_file(const std::string& filename)
{
    const std::ifstream input_stream(filename, std::ios_base::binary);

    if (input_stream.fail()) {
        throw std::runtime_error("Failed to open file");
    }

    std::stringstream buffer;
    buffer << input_stream.rdbuf();

    auto result = buffer.str();
    result.erase(
        std::remove_if(result.begin(), result.end(), isspace), result.end());
    return result;
}

} // namespace FileSystem