
#include "Form.hpp"

#include "CheckBoxSelect.hpp"
#include "Data/Record.hpp"
#include "Hidden.hpp"
#include "Submit.hpp"
#include "Text.hpp"

#include <sstream>

using std::ostringstream;
namespace Input {

Form::Form(vector<string> elements, string action, string method)
    : m_elements(move(elements))
    , m_action(move(action))
    , m_method(move(method))
{
}
Form::Form(const Record& record, string action, string method)
    : m_action(move(action))
    , m_method(move(method))
{
    m_elements.push_back(Hidden("id", record.key())());
    for (const auto& [key, value] : record.values()) {

        switch (record.inputType(key)) {
        case HtmlInputType::HIDDEN:
            m_elements.push_back(Hidden(key, value)());
            break;
        case HtmlInputType::CHECKBOX:
            m_elements.push_back(CheckBoxSelect(key, value)());
            break;
        default:
            m_elements.push_back(Text(key, value)());
        }
    }
}
string Form::operator()()
{
    ostringstream str;
    str << R"(<form action=")" << m_action << R"(" method=")" << m_method
        << R"(">)";
    for (auto element : m_elements) {
        str << element << "<br>\n";
    }
    str << "</form>";
    return str.str();
}
Form& Form::appendElement(string element)
{
    m_elements.push_back(std::move(element));
    return *this;
}
} // namespace Input
