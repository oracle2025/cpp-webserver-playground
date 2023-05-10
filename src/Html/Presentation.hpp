#pragma once

#include "Footer.hpp"
#include "Header.hpp"
#include "Response.hpp"

#include <sstream>
#include <string>

using std::ostringstream;
namespace Html {

struct Presentation {
    static string render(const Response& response);
    static string renderAlert(const Alert& alert);
    static string renderActions(const vector<ActionLink>& actions);
    static string renderNavBarActions(const vector<ActionLink>& actions);
    static string renderHtml(const Response& response);
};
} // namespace Html