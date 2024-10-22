#pragma once

#include "Element.hpp"

namespace Input {

struct Select: public Element {
    explicit Select(string label, const std::vector<std::string> &options);

    string operator()() override;
    string name() const override;
    string value() const override;
    void value(const string& content) override;

private:
    const std::vector<std::string> m_options;
    const string m_label;
    string m_value;
};

} // namespace Input

