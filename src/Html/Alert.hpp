#pragma once

#include "AlertType.hpp"

#include <string>
using std::string;
namespace Html {

class Alert {
public:
    explicit Alert(const string& alert, AlertType type = AlertType::DANGER);
    Alert() = default;
    string message() const;
    AlertType alertType() const;
    string typeAsString() const;
    static AlertType fromString(const string& alertStr);

private:
    string alert;
    AlertType type;
};

} // namespace Html
