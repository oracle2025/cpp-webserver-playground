
#include "Form.hpp"

#include "CheckBoxSelect.hpp"
#include "Data/Record.hpp"
#include "Date.h"
#include "TimeElement.h"
#include "Hidden.hpp"
#include "Text.hpp"
#include "Poco/URI.h"
#include <sstream>

using std::ostringstream;
namespace Input {

Form::Form(const Record& record, string action, string method)
    : m_action(std::move(action))
    , m_method(std::move(method))
{
    using std::make_shared;
    m_pureElements.push_back(make_shared<Hidden>("id", record.key()));
    for (const auto& [key, value] : record.values()) {

        switch (record.inputType(key)) {
        case HtmlInputType::HIDDEN:
            m_pureElements.push_back(make_shared<Hidden>(key, value));
            break;
        case HtmlInputType::CHECKBOX:
            m_pureElements.push_back(make_shared<CheckBoxSelect>(key, value));
            break;
        case HtmlInputType::DATE:
            m_pureElements.push_back(make_shared<Date>(key, value));
            break;
        case HtmlInputType::TIME:
            m_pureElements.push_back(make_shared<TimeElement>(key, value));
            break;
        case HtmlInputType::TEXT:
        case HtmlInputType::NUMBER:
        case HtmlInputType::DATETIME:
        case HtmlInputType::RADIO:
        case HtmlInputType::TEXTAREA:
        case HtmlInputType::SELECT:
        default:
            m_pureElements.push_back(make_shared<Text>(key, value));
            break;
        case HtmlInputType::NON_EDITABLE:
            break;
        }
    }
}
Form::Form(vector<ElementPtr> elements, string action, string method)
    : m_pureElements(std::move(elements))
    , m_action(std::move(action))
    , m_method(std::move(method))
{
}
Form::Form(const string& element, string action, string method)
    : m_action(std::move(action))
    , m_method(std::move(method))
{
    m_elements.push_back(element);
}
string Form::operator()()
{
    R"(      <form class="form-signin">
        <h2 class="form-signin-heading">Login</h2>

        <label for="inputEmail" class="sr-only">Email address</label>
        <input type="email" id="inputEmail" class="form-control" placeholder="Email address" required autofocus>

        <label for="inputPassword" class="sr-only">Password</label>
        <input type="password" id="inputPassword" class="form-control" placeholder="Password" required>

        <button class="btn btn-lg btn-primary btn-block" type="submit">Sign in</button>
      </form>)";
    ostringstream str;
    str << R"(<form action=")" << m_action << R"(" method=")" << m_method
        << R"(">)";
    for (const auto& element : m_elements) {
        str << element << "<br>\n";
    }
    for (const auto& element : m_pureElements) {
        str << (*element)() << "<br>\n";
    }
    str << "</form>";
    return str.str();
}
Form& Form::appendElement(string element)
{
    m_elements.push_back(std::move(element));
    return *this;
}
Form& Form::appendElement(ElementPtr element)
{
    m_pureElements.push_back(std::move(element));
    return *this;
}
string Form::name() const
{
    return "";
}
string Form::value() const
{
    return "";
}
string Form::action() const
{
    return m_action;
}
string Form::data() const
{
    ostringstream str;
    for (auto& element : m_pureElements) {
        std::string value;
        Poco::URI::encode(element->value(), "/?&:", value);
        str << element->name() << "=" << value << "&";
    }
    return str.str();
}
void Form::set(const string& key, const string& value)
{
    for (auto& element : m_pureElements) {
        if (element->name() == key) {
            element->value(value);
            return;
        }
    }
}
void Form::value(const string& content)
{
}

} // namespace Input
