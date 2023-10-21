#pragma once

#include "Element.hpp"

namespace Input {

struct Password : public Element {
    explicit Password(string label);

    string operator()() override;
    string name() const override;
    string value() const override;
    void value(const string& content) override;

private:
    string m_label;
    string m_value;
};

} // namespace Input
