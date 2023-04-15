#pragma once

#include "Record.hpp"
#include <sstream>
#include <string>


namespace Html {
using std::ostringstream;
using std::string;

class Table {
public:
    Table(const Record &record) : record(record) {}

    string operator()() {
        ostringstream str;
        str << R"(<table border="1">)";
        for (const auto &[key, value]: record.values()) {
            str << "<tr><th>" << key << "</th><td>" << value << "</td></tr>\n";
        }
        str << "</table>\n";
        return str.str();
    }

private:
    const Record &record;
};

}// namespace Html
