

#include "Submit.hpp"

#include <sstream>

using std::ostringstream;
namespace Input {
Submit::Submit(string label)
    : m_label(move(label))
{
}
string Submit::operator()()
{
    ostringstream str;
    string name;
    string value;
    if (m_name.empty()) {
        name = R"(" name=")" + m_label + R"(")";
    } else {
        name = R"(" name=")" + m_name + R"(")";
    }
    if (!m_value.empty()) {
        value = R"(" value=")" + m_value + R"(")";
    }

    str << R"(<button type="submit" )" << value << R"( id=")" << m_label
        << R"(" )" << name << R"(>)" << m_label << R"(</button>)";

    return str.str();
}
Submit& Submit::value(const string& value)
{
    m_value = value;
    return *this;
}
Submit& Submit::name(const string& value)
{
    m_name = value;
    return *this;
}
} // namespace Input