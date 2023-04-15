#pragma once

#include "Record.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace Html {
using std::ostringstream;
using std::string;
using std::vector;

class List {
public:
    List(vector<std::shared_ptr<Record>> records) : records(std::move(records)) {}
    string operator()() {
        ostringstream str;
        str << R"(<table border="1">)";
        str << "<tr>";
        if (!records.empty()) {
            for (auto &col: (*records.begin())->fields()) {
                str << "<th>" << col << "</th>";
            }
        }
        str << "</tr>";
        for (auto record: records) {
            str << "<tr>";
            auto values = record->values();
            for (const auto &column: record->fields()) {
                str << R"(<td><a href="/edit?)" << values["id"] << R"(">)" << values[column] << "</a></td>";
            }
            str << "</tr>";
        }
        str << "</table>\n";
        return str.str();
    }

private:
    vector<std::shared_ptr<Record>> records;
};

}// namespace Html
