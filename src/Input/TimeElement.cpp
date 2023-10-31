#include "TimeElement.h"
#include "String/escape.hpp"

#include <sstream>
#include <algorithm>

namespace Input {

using std::ostringstream;
using std::transform;

TimeElement::TimeElement(string label, string value)
    : m_label(std::move(label))
    , m_value(std::move(value))
{
}
string TimeElement::operator()()
{
    string capitalized = m_label;
    transform(
        capitalized.begin(),
        capitalized.begin() + 1,
        capitalized.begin(),
        ::toupper);

    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << capitalized
        << R"(</label><br> <input type="time" step="3600" id=")" << m_label << R"(" name=")"
        << m_label << R"(" value=")" << String::escape(m_value) << R"(">)";

    return str.str();
}
string TimeElement::name() const
{
    return m_label;
}
string TimeElement::value() const
{
    return m_value;
}
void TimeElement::value(const string& content)
{
    m_value = content;
}

} // namespace Input