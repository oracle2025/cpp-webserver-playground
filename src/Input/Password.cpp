
#include "Password.hpp"

#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {

string Password::operator()()
{
    string capitalized = m_label;
    transform(
        capitalized.begin(),
        capitalized.begin() + 1,
        capitalized.begin(),
        ::toupper);

    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << capitalized
        << R"(</label><br> <input type="password" id=")" << m_label
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
