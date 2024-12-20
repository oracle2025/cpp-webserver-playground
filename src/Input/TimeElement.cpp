#include "TimeElement.hpp"

#include "String/capitalize.hpp"
#include "String/escape.hpp"

#include <algorithm>
#include <sstream>

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
    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << String::capitalize(m_label)
        << R"(</label><br> <input class="form-control" type="time" step="60" id=")" << m_label << R"(" name=")"
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