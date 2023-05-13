#pragma once

#include "Response.hpp"

#include <sstream>
#include <string>

using std::ostringstream;
using std::vector;
using Http::Response;
namespace Html {
struct Presentation {
    static string render(const Response& response);
    static string renderAlert(const Alert& alert);
    static string renderActions(const vector<ActionLink>& actions);
    static string renderNavBarActions(const vector<ActionLink>& actions);
    static string renderHtml(const Response& response);
};
} // namespace Html