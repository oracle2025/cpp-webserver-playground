#pragma once

#include <string>
#include <memory>

using std::string;

namespace Input {

struct Element {
    virtual ~Element() = default;
    virtual string operator()() = 0;

    virtual string name() const = 0;
    virtual string value() const = 0;
    virtual void value(const string& content) = 0;
};

using ElementPtr = std::shared_ptr<Element>;

} // namespace Input
