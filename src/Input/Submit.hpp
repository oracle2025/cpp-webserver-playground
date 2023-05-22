#pragma once

#include "Element.hpp"

namespace Input {

struct Submit : public Element {
    explicit Submit(string label);

    string operator()();
    Submit& value(const string& value);
    Submit& name(const string& value);

private:
    string m_label;
    string m_value;
    string m_name;
};

} // namespace Input
