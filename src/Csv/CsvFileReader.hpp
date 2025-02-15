#pragma once

#include <string>
#include <vector>

namespace Csv {

class CsvFileReader {
public:
    explicit CsvFileReader(std::string  filename);

    std::vector<std::vector<std::string>> get() const;
private:
    std::string m_filename;
};

} // namespace Csv

