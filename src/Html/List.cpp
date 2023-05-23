

#include "List.hpp"

#include <sstream>

namespace Html {
using std::ostringstream;

string List::operator()()
{
    ostringstream str;
    str << R"(<table>)";
    int even = 0;
    for (auto record : records) {
        if (even % 2) {
            str << R"(<tr class="uneven">)";
        } else {
            str << "<tr>";
        }
        even++;
        auto values = record->values();
        for (const auto& column : columns) {
            switch (record->inputType(column)) {
            case HtmlInputType::CHECKBOX:
                if (values[column] == "yes") {
                    str << R"(<td style="width: 20px;"><input type="checkbox" checked></td>)";
                } else {
                    str << R"(<td style="width: 20px;"><input type="checkbox"></td>)";
                }
                break;
            default:
                str << R"(<td class="max">)" << values[column] << "</td>";
                break;
            }
        }
        str << R"(<td><a href=")" << m_prefix << R"(/edit?)" << record->key()
            << R"(" class="edit button">)"
            << R"(✏️ <span class="label">Edit</span></a>)"
               "</td>";
        str << R"(<td><a href=")" << m_prefix << R"(/delete?)" << record->key()
            << R"(" class="remove button">)"
            << R"(♻️ <span class="label">Delete</span></a>)"
               "</td>";
        str << "</tr>";
    }
    str << "</table>\n";
    return str.str();
}
List::List(vector<shared_ptr<Record>> records, vector<string> columns)
    : records(std::move(records))
    , columns(std::move(columns))
{
}
List& List::prefix(const string& prefix)
{
    m_prefix = prefix;
    return *this;
}
} // namespace Html