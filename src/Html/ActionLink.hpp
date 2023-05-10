#pragma once
#include <string>
using std::string;

struct ActionLink {
    ActionLink(
        const string& title, const string& url, const string& liClass = "")
        : title(title)
        , url(url)
        , liClass(liClass)
    {
    }

    string title;
    string url;
    string liClass;
};