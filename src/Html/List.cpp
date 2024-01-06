

#include "List.hpp"

#include "Data/ScaffoldRecord.hpp"
#include "String/escape.hpp"

#include <doctest.h>

#include <sstream>

namespace Html {
using std::ostringstream;

TEST_CASE("Html::List")
{
    SUBCASE("empty")
    {
        List list({}, {});
        CHECK(list() == R"(<table class="table"></table>)");
    }
    SUBCASE("one record")
    {
        List list(
            {std::make_shared<Data::ScaffoldRecord>(
                 "1", Data::ScaffoldRecord::FieldsType{{"name", HtmlInputType::TEXT}})},
            {"name"});
        CHECK(list() == R"(<table class="table"><tr><td class="max">John</td>=)"
                         R"(<td><a href="/edit?1" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>=)"
                         R"(<td><a href="/delete?1" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a></td></tr></table>)");
    }
}

string List::operator()()
{
    ostringstream str;
    str << R"(<table class="table">)";
    if (m_withHeader) {
        str << R"(<thead><tr>)";
        for (const auto& column : columns) {
            str << R"(<th>)" << column << "</th>";
        }
        str << "</tr></thead>\n";
    }
    int even = 0;
    for (const auto& record : records) {
        if (even % 2) {
            str << R"(<tr class="uneven">)";
        } else {
            str << "<tr>\n";
        }
        even++;
        auto values = record->values();
        for (const auto& column : columns) {
            switch (record->inputType(column)) {
            case HtmlInputType::CHECKBOX:
                str << R"(<td style="width: 20px;">)";
                str << R"(<input type="hidden" name=")" << record->key()
                    << R"(" value="no" />)";
                if (values[column] == "yes") {
                    str << R"(<input type="checkbox" name=")" << record->key()
                        << R"(" checked value="yes" onchange="submitForm(this);"></td>)";
                } else {
                    str << R"(<input type="checkbox" name=")" << record->key()
                        << R"(" value="yes" onchange="submitForm(this);"></td>)";
                }
                break;
            case HtmlInputType::TEXT:
            case HtmlInputType::NUMBER:
            case HtmlInputType::DATE:
            case HtmlInputType::DATETIME:
            case HtmlInputType::TIME:
            case HtmlInputType::RADIO:
            case HtmlInputType::TEXTAREA:
            case HtmlInputType::SELECT:
            case HtmlInputType::HIDDEN:
            default:
                str << R"(<td class="max">)" << String::escape(values[column])
                    << "</td>\n";
                break;
            }
        }
        str << R"(<td><a href=")" << m_prefix << R"(/edit?)" << record->key()
            << R"(" class="edit button btn btn-success btn-sm">)"
            << R"(✏️ <span class="hidden-xs">Edit</span></a>)"
               "</td>\n";
        str << R"(<td><a href=")" << m_prefix << R"(/delete?)" << record->key()
            << R"(" class="remove button btn btn-danger btn-sm">)"
            << R"(♻️ <span class="hidden-xs">Delete</span></a>)"
               "</td>\n";
        str << "</tr>\n";
    }
    str << "</table>\n";
    return str.str();
}
List::List(vector<shared_ptr<Record>> records, vector<KeyStringType> columns)
    : records(std::move(records))
    , columns(std::move(columns))
{
}
List& List::prefix(const string& prefix)
{
    m_prefix = prefix;
    return *this;
}
List& List::withHeader()
{
    m_withHeader = true;
    return *this;
}
} // namespace Html