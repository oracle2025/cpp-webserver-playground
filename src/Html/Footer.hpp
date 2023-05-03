#pragma once
#include <string>
using std::string;
struct Footer {
    string m_content;
    Footer(const string& content)
        : m_content(content)
    {
    }
};