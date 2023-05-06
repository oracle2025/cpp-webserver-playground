#pragma once

#include "Element.hpp"

namespace Input {

class CheckBoxSelect  : public Element
{
public:
    CheckBoxSelect(string label, string value = "yes");
    string operator()();
private:
    string m_label;
    string m_value;
};

} // namespace Input
