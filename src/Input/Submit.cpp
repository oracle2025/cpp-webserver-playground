

#include "Submit.hpp"
#include "doctest.h"

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
    string buttonClass;
    if (m_name.empty()) {
        name = R"(" name=")" + m_label + R"(")";
    } else {
        name = R"(" name=")" + m_name + R"(")";
    }
    if (!m_value.empty()) {
        value = R"(" value=")" + m_value + R"(")";
    }

    if (m_class.empty()) {
        buttonClass = "btn btn-success";
    } else {
        buttonClass = "btn btn-" + m_class;
    }

    str << R"(<button class=")" << buttonClass << R"(" type="submit" )" << value
        << R"( id=")" << m_label << R"(" )" << name << R"(>)" << m_label
        << R"(</button>)";

    return str.str();
}
Submit& Submit::valueP(const string& value)
{
    m_value = value;
    return *this;
}
Submit& Submit::name(const string& value)
{
    m_name = value;
    return *this;
}
Submit& Submit::buttonClass(const string& value)
{
    m_class = value;
    return *this;
}
string Submit::name() const
{
    return m_label;
}
string Submit::value() const
{
    return m_value;
}
void Submit::value(const string& content)
{
    m_value = content;
}
} // namespace Input

TEST_CASE("Submit Button Test") {
    //CHECK(Submit("Login")() == "")
}