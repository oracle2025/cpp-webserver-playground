#pragma once

#include "Element.hpp"
#include <vector>

using std::vector;
class Record;
namespace Input {
struct Form : public Element {
    Form(vector<string> elements, string action, string method);

    Form(const Record &record, string action, string method);

    Form& appendElement(string element);

    string operator()();

private:
    vector<string> m_elements;
    string m_action;
    string m_method;
};
}// namespace Input