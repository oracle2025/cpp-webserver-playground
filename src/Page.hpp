//
// Created by Richard Spindler on 12.02.23.
//

#ifndef CPP_WEBSERVER_KATA_PAGE_HPP
#define CPP_WEBSERVER_KATA_PAGE_HPP

#include <string>

class Page {
public:
    Page(std::string title, std::string content);
    std::string get() const;
private:
    std::string m_title;
    std::string m_content;
};


#endif //CPP_WEBSERVER_KATA_PAGE_HPP
