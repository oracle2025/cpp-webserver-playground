#include "Alert.hpp"

#include <unordered_map>
namespace Html {
Alert::Alert(const string& alert, AlertType type)
    : alert(alert)
    , type(type)
{
}
string Alert::message() const
{
    return alert;
}
AlertType Alert::alertType() const
{
    return type;
}
string Alert::typeAsString() const
{
    switch (type) {
    case AlertType::PRIMARY:
        return "alert-primary";
    case AlertType::SECONDARY:
        return "alert-secondary";
    case AlertType::SUCCESS:
        return "alert-success";
    case AlertType::DANGER:
        return "alert-danger";
    case AlertType::WARNING:
        return "alert-warning";
    case AlertType::INFO:
        return "alert-info";
    }
    return "alert-danger";
}
AlertType Alert::fromString(const string& alertStr)
{
    using std::unordered_map;
    unordered_map<string, AlertType> mapping
        = {{"alert-primary", AlertType::PRIMARY},
           {"alert-secondary", AlertType::SECONDARY},
           {"alert-success", AlertType::SUCCESS},
           {"alert-danger", AlertType::DANGER},
           {"alert-warning", AlertType::WARNING},
           {"alert-info", AlertType::INFO}};
    if (mapping.count(alertStr)) {
        return mapping.at(alertStr);
    }
    return AlertType::DANGER;
}
} // namespace Html