#pragma once

#include "nlohmann/json.hpp"

namespace Template {

template <typename T>
nlohmann::json::array_t selectFromList(const std::vector<T>& list, const T& selected, const std::string& name = "item")
{
    nlohmann::json::array_t list_with_selection;
    for (const auto& item : list) {
        nlohmann::json item_with_selection;
        item_with_selection[name] = item;
        if (item == selected) {
            item_with_selection["selected"] = true;
        } else {
            item_with_selection["selected"] = false;
        }
        list_with_selection.push_back(item_with_selection);
    }
    return list_with_selection;
}

}