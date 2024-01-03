#include "CheckBoxSelect.hpp"

#include "String/capitalize.hpp"
#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {
CheckBoxSelect::CheckBoxSelect(string label, string value)
    : m_label(std::move(label))
    , m_value(std::move(value))
{
}
string CheckBoxSelect::operator()()
{
    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << String::capitalize(m_label)
        << R"(</label><br>)";

    str << R"(<select class="form-control" name=")" << m_label << R"(" id=")" << m_label << R"(">)";
    if (m_value == "yes") {
        str << R"(<option value="yes" selected>Yes</option><option value="no">No</option>)";

    } else {
        str << R"(<option value="yes">Yes</option><option value="no" selected>No</option>)";
    }
    str << R"(</select>)";
    return str.str();
}
string CheckBoxSelect::name() const
{
    return m_label;
}
string CheckBoxSelect::value() const
{
    return m_value;
}
void CheckBoxSelect::value(const string& content)
{
    m_value = content;
}
} // namespace Input