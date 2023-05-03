#pragma once

#include "Data/Record.hpp"

#include <sstream>
#include <string>

namespace Html {
using std::ostringstream;
using std::string;

class Table {
public:
    explicit Table(const Record& record);

    string operator()();

private:
    const Record& record;
};

} // namespace Html
