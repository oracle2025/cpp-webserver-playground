#pragma once

#include "Element.hpp"

namespace Input {

struct Submit : public Element {
    explicit Submit(string label);

    string operator()();

private:
    string m_label;
};

}// namespace Input

