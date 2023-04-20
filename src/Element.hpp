#pragma once

#include <string>

using std::string;

namespace Input {

struct Element {
    virtual string operator()() = 0;
};

}// namespace Input
