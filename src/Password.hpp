#pragma once

#include "Element.hpp"

namespace Input {

struct Password : public Element {
    explicit Password(string label);

    string operator()();

private:
    string m_label;
};

} // namespace Input
