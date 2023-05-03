#pragma once

#include "Footer.hpp"
#include "Header.hpp"

#include <string>

using std::string;

struct Presentation {
    Header m_header;
    Footer m_footer;
    Presentation(const Header& header, const Footer& footer)
        : m_header(header)
        , m_footer(footer)
    {
    }
    string render(const string& content)
    {
        return m_header.m_content + content + m_footer.m_content;
    }
};