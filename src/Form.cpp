
#include "Form.hpp"
#include "Submit.hpp"
#include "Text.hpp"
#include "Hidden.hpp"
#include "Record.hpp"
#include <sstream>

using std::ostringstream;
namespace Input {

Form::Form(vector<string> elements, string action, string method) : m_elements(move(elements)),
                                                            m_action(move(action)),
                                                            m_method(move(method)) {}
Form::Form(const Record &record, string action, string method) : m_action(move(action)),
                                                                 m_method(move(method)) {
    m_elements.push_back(Hidden("m_id", record.id())());
    for (const auto &[key, value]: record.values()) {
        m_elements.push_back(Text(key, value)());
    }
    m_elements.push_back(Submit("submit")());
}
string Form::operator()() {
    ostringstream str;
    str << R"(<form action=")" << m_action << R"(" method=")" << m_method << R"(">)";
    for (auto element: m_elements) {
        str << element << "<br>\n";
    }
    str << "</form>";
    return str.str();
}
}// namespace Input
