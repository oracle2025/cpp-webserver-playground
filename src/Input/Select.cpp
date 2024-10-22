#include "Select.hpp"

#include "String/capitalize.hpp"

#include <sstream>
#include <utility>

namespace Input {

Select::Select(string label, const std::vector<std::string>& options)
    : m_options(options)
    , m_label(std::move(label))
    , m_value(options.front())
{
}

string Select::operator()()
{
    std::ostringstream str;
    str << R"(<label for=")" << m_label << R"(">)"
        << String::capitalize(m_label) << R"(</label><br>)" << "\n";
    str << R"(<select class="form-control" name=")" << m_label << R"(" id=")"
        << m_label << R"(">)" << "\n";
    for (const auto& option : m_options) {
        str << R"(<option value=")" << option << R"(">)"
            << String::capitalize(option) << R"(</option>)" << "\n";
    }
    str << R"(</select>)" << "\n";
    return str.str();
}
string Select::name() const
{
    return m_label;
}
string Select::value() const
{
    return m_value;
}
void Select::value(const string& content)
{
    m_value = content;
}

} // namespace Input