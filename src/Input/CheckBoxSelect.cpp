#include "CheckBoxSelect.hpp"

#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {
CheckBoxSelect::CheckBoxSelect(string label, string value)
    : m_label(move(label))
    , m_value(move(value))
{
}
string CheckBoxSelect::operator()()
{
    string capitalized = m_label;
    transform(
        capitalized.begin(),
        capitalized.begin() + 1,
        capitalized.begin(),
        ::toupper);

    ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)" << capitalized
        << R"(</label><br>)";

    str << R"(<select name=")" << m_label << R"(" id=")" << m_label << R"(">)";
    if (m_value == "yes") {
        str << R"(<option value="yes" selected>Yes</option><option value="no">No</option>)";

    } else {
        str << R"(<option value="yes">Yes</option><option value="no" selected>No</option>)";
    }
    str << R"(</select>)";
    return str.str();
}
} // namespace Input