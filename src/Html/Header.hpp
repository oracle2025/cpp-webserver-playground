#pragma once
#include <string>
using std::string;
struct Header {
    string m_content;
    Header(const string& content)
        : m_content(content)
    {
    }
};

