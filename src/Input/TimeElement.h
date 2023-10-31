#pragma once

#include "Element.hpp"

namespace Input {

class TimeElement : public Element {
public:
    explicit TimeElement(string label, string value = "");

    string operator()() override;

    string name() const override;
    string value() const override;
    void value(const string& content) override;

private:
    string m_label;
    string m_value;
};

} // namespace Input
