#include "Table.hpp"

namespace Html {
string Table::operator()()
{
    ostringstream str;
    str << R"(<table border="1">)";
    for (const auto& [key, value] : record.values()) {
        str << "<tr><th>" << key << "</th><td>" << value << "</td></tr>\n";
    }
    str << "</table>\n";
    return str.str();
}
Table::Table(const Record& record)
    : record(record)
{
}
} // namespace Html