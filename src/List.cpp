

#include "List.hpp"

#include <sstream>

namespace Html {
using std::ostringstream;

string List::operator()()
{
    ostringstream str;
    str << R"(<table border="1">)";
    str << "<tr>";
    if (!records.empty()) {
        for (auto& col : (*records.begin())->fields()) {
            str << "<th>" << col << "</th>";
        }
    }
    str << "</tr>";
    for (auto record : records) {
        str << "<tr>";
        auto values = record->values();
        for (const auto& column : record->fields()) {
            str << R"(<td><a href="/edit?)" << values["m_id"] << R"(">)"
                << values[column] << "</a></td>";
        }
        str << "</tr>";
    }
    str << "</table>\n";
    return str.str();
}
List::List(vector<shared_ptr<Record>> records)
    : records(std::move(records))
{
}
} // namespace Html