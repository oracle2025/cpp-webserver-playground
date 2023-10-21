#pragma once

#include "Element.hpp"

namespace Input {

class CheckBoxSelect : public Element {
public:
    CheckBoxSelect(string label, string value = "yes");
    string operator()() override;
    string name() const override;
    string value() const override;
    void value(const string& content) override;

private:
    string m_label;
    string m_value;
};

} // namespace Input
