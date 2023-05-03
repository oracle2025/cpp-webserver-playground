#pragma once
#include <string>

class Page {
public:
    Page(std::string title, std::string content);
    std::string get() const;

private:
    std::string m_title;
    std::string m_content;
};
