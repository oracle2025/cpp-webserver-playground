#include "AsJson.hpp"
#include "Record.hpp"
#include "FieldTypes.hpp"

namespace Data {
nlohmann::json AsJson::fromRecord(const Record& record)
{
    nlohmann::json data;
    data["id"] = record.key();
    for (const auto& [key, value] : record.values()) {
        switch (record.inputType(key)) {
        case HtmlInputType::CHECKBOX:
            data[key + "_Yes"] = data[key + "_No"] = "";
            if (value == "yes") {
                data[key + "_Yes"] = "selected";
            } else {
                data[key + "_No"] = "selected";
            }
            break;
        case HtmlInputType::HIDDEN:
        case HtmlInputType::DATE:
        case HtmlInputType::TIME:
        case HtmlInputType::TEXT:
        case HtmlInputType::NUMBER:
        case HtmlInputType::DATETIME:
        case HtmlInputType::RADIO:
        case HtmlInputType::TEXTAREA:
        case HtmlInputType::SELECT:
        case HtmlInputType::NON_EDITABLE:
        default:
            data[key] = value;
        }
    }
    return data;
}
} // Data