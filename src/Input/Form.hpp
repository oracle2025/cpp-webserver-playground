#pragma once

#include "Element.hpp"

#include <vector>

using std::vector;
class Record;
namespace Input {
struct Form : public Element {
    Form(vector<string> elements, string action, string method);
    Form(vector<ElementPtr> elements, string action, string method);

    Form(const Record& record, string action, string method);

    void set(const string& key, const string& value);

    Form& appendElement(string element);
    Form& appendElement(ElementPtr element);

    string operator()() override;

    string name() const override;
    string value() const override;
    void value(const string& content) override;

    string action() const;

    string data() const; // "username=admin&password=Adm1n!"

private:
    vector<string> m_elements;
    vector<ElementPtr> m_pureElements;
    string m_action;
    string m_method;
};
using FormPtr = std::shared_ptr<Form>;
} // namespace Input