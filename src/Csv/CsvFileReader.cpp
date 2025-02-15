#include "CsvFileReader.hpp"
#include "String/split.hpp"

#include <fstream>
#include <utility>

namespace Csv {
CsvFileReader::CsvFileReader(std::string filename)
    : m_filename{std::move(filename)}
{
}
std::vector<std::vector<std::string>> CsvFileReader::get() const
{
    std::vector<std::vector<std::string>> result;
    std::ifstream in(m_filename);
    for (std::string line; std::getline(in, line);) {
        result.push_back(String::split(line, ","));
    }
    return result;
}
} // namespace Csv