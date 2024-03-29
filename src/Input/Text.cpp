
#include "Text.hpp"

#include "String/capitalize.hpp"
#include "String/escape.hpp"

#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {
string Text::operator()()
{
    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << String::capitalize(m_label)
        << R"(</label><br> <input class="form-control" type="text" id=")" << m_label << R"(" name=")"
        << m_label << R"(" value=")" << String::escape(m_value) << R"(">)";
    return str.str();
}
Text::Text(string label, string value)
    : m_label(std::move(label))
    , m_value(std::move(value))
{
}
string Text::name() const
{
    return m_label;
}
string Text::value() const
{
    return m_value;
}
void Text::value(const string& content)
{
    m_value = content;
}

} // namespace Input