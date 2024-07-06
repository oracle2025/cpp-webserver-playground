#pragma once

#include "Response.hpp"

#include <sstream>
#include <string>

using Http::Response;
using std::ostringstream;
using std::vector;
namespace Html {
struct Presentation {
    explicit Presentation(const std::string& applicationName);
    string render(const Response& response);
    static string renderAlert(const Alert& alert);
    static string renderActions(const vector<ActionLink>& actions);
    static string renderNavBarActions(const vector<ActionLink>& actions);
    string renderHtml(const Response& response);

private:
    const std::string m_applicationName;
};
} // namespace Html