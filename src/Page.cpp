//
// Created by Richard Spindler on 12.02.23.
//

#include "Page.hpp"
#include <sstream>
#include <utility>

Page::Page(std::string title, std::string content) :
m_title(std::move(title)), m_content(std::move(content)){

}

std::string Page::get() const {
    std::ostringstream str;
    str << R"(<!doctype html><html lang="de"><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">)";
    str << "<head>";
    str << "<title>" << m_title << "</title>";
    str << "</head>";
    str << "<h1>" << m_title << "</h1>";
    str << "<body>" << m_content << "</body>";
    str << "</html>";
    return str.str();
}
