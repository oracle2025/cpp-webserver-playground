#pragma once

#include "Element.hpp"

namespace Input {

struct Text : public Element {
    explicit Text(string label, string value = "");

    string operator()();

private:
    string label;
    string value;
};

} // namespace Input
