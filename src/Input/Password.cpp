
#include "Password.hpp"

#include <sstream>

using std::ostringstream;
namespace Input {

string Password::operator()()
{
    ostringstream str;
    str << R"(<label for =")" << m_label << R"(">)" << m_label
        << R"(</label><br> <input type="password" m_id=")" << m_label
        << R"(" title=")" << m_label << R"(" value=")"
        << ""
        << R"(">)";
    return str.str();
}
Password::Password(string label)
    : m_label(move(label))
{
}

} // namespace Input
