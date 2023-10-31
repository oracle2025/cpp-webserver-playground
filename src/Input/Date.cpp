#include "Input/Date.h"

#include "String/escape.hpp"

#include <sstream>

namespace Input {

using std::ostringstream;

Date::Date(string label, string value)
    : m_label(std::move(label))
    , m_value(std::move(value))
{
}
string Date::operator()()
{
    string capitalized = m_label;
    transform(
        capitalized.begin(),
        capitalized.begin() + 1,
        capitalized.begin(),
        ::toupper);
    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << capitalized
        << R"(</label><br> <input type="date" id=")" << m_label << R"(" name=")"
        << m_label << R"(" value=")" << String::escape(m_value) << R"(">)";
    return str.str();
}
string Date::name() const
{
    return m_label;
}
string Date::value() const
{
    return m_value;
}
void Date::value(const string& content)
{
    m_value = content;
}

} // namespace Input