#include "Hidden.hpp"

#include "String/escape.hpp"

#include <sstream>

namespace Input {

using std::ostringstream;

string Hidden::operator()()
{
    ostringstream str;
    str << R"(<input type="hidden" id=")" << m_label << R"(" name=")" << m_label
        << R"(" value=")" << String::escape(m_value) << R"(">)";
    return str.str();
}
Hidden::Hidden(string label, string value)
    : m_label(std::move(label))
    , m_value(std::move(value))
{
}
string Hidden::value() const
{
    return m_value;
}
string Hidden::name() const
{
    return m_label;
}
void Hidden::value(const string& content)
{
    m_value = content;
}
} // namespace Input