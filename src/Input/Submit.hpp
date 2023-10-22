#pragma once

#include "Element.hpp"

namespace Input {

using std::enable_shared_from_this;

struct Submit : public Element, enable_shared_from_this<Submit> {
    explicit Submit(string label);

    string operator()() override;
    Submit& valueP(const string& value);
    Submit& name(const string& value);
    Submit& buttonClass(const string& value);
    string name() const override;
    string value() const override;
    void value(const string& content) override;

private:
    string m_label;
    string m_value;
    string m_name;
    string m_class;
};

} // namespace Input
