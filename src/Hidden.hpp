#pragma once

#include "Element.hpp"

namespace Input {

struct Hidden : public Element {
    explicit Hidden(string label, string value = "");

    string operator()();

private:
    string m_label;
    string m_value;
};

}// namespace Input

