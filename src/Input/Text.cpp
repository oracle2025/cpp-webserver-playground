
#include "Text.hpp"

#include "String/escape.hpp"

#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {
string Text::operator()()
{
    string capitalized = m_label;
    transform(
        capitalized.begin(),
        capitalized.begin() + 1,
        capitalized.begin(),
        ::toupper);

    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << capitalized
        << R"(</label><br> <input type="text" id=")" << m_label << R"(" name=")"
        << m_label << R"(" value=")" << String::escape(m_value) << R"(">)";
    return str.str();
}
Text::Text(string label, string value)
    : m_label(move(label))
    , m_value(move(value))
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