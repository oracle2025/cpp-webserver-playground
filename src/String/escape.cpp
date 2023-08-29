#include "escape.hpp"

namespace String {

std::string escape(const std::string& s)
{
    std::ostringstream o;
    for (auto c : s) {
        switch (c) {
        case '&':  o << "&amp;";       break;
        case '\"': o << "&quot;";      break;
        case '\'': o << "&apos;";      break;
        case '<':  o << "&lt;";        break;
        case '>':  o << "&gt;";        break;
        default:   o << c;             break;
        }
    }
    return o.str();
}
}