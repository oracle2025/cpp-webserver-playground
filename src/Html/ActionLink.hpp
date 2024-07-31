#pragma once
#include <string>
using std::string;

struct ActionLink;

struct ActionLink {
    ActionLink(
        const string& title,
        const string& url,
        const string& liClass = "",
        const std::vector<ActionLink>& dropDowns = {})
        : title(title)
        , url(url)
        , liClass(liClass)
        , dropDowns(dropDowns)
    {
    }

    string title;
    string url;
    string liClass;
    std::vector<ActionLink> dropDowns = {};
};