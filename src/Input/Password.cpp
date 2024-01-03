
#include "Password.hpp"
#include "String/capitalize.hpp"

#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {

string Password::operator()()
{
    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << String::capitalize(m_label)
        << R"(</label><br> <input class="form-control" type="password" id=")" << m_label
        << R"(" name=")" << m_label << R"(" value=")"
        << ""
        << R"(">)";
    return str.str();
}
Password::Password(string label)
    : m_label(std::move(label))
{
}
string Password::name() const
{
    return m_label;
}
string Password::value() const
{
    return m_value;
}
void Password::value(const string& content)
{
    m_value = content;
}

} // namespace Input
