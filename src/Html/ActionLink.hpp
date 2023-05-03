#pragma once
#include <string>
using std::string;

struct ActionLink {
        ActionLink(const string& title, const string& url)
                : title(title)
                , url(url)
        {
        }

        string title;
        string url;
};