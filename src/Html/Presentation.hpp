#pragma once

#include "Footer.hpp"
#include "Header.hpp"
#include "Response.hpp"

#include <sstream>
#include <string>

using std::ostringstream;
namespace Html {

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
    string render(const Response& response)
    {
        if (response.mimetype() == "text/html") {
            return renderHtml(response);
        }
        return response.content();
    }
    string renderAlert(const Alert& alert)
    {
        if (alert.message().empty()) {
            return "";
        }
        string alertClass = "alert-danger";
        switch (alert.alertType()) {
        case AlertType::PRIMARY:
            alertClass = "alert-primary";
            break;
        case AlertType::SECONDARY:
            alertClass = "alert-secondary";
            break;
        case AlertType::SUCCESS:
            alertClass = "alert-success";
            break;
        case AlertType::DANGER:
            alertClass = "alert-danger";
            break;
        case AlertType::WARNING:
            alertClass = "alert-warning";
            break;
        case AlertType::INFO:
            alertClass = "alert-info";
            break;
        }
        return R"(<div class=")" + alertClass + R"(">⚠️ )" + alert.message()
            + R"(</div>)";
    }
    string renderActions(const vector<ActionLink>& actions)
    {
        if (actions.empty()) {
            return "";
        }
        ostringstream result;
        for (const auto& action : actions) {
            result << R"(<a href=")" << action.url
                   << R"(" class="create button">)" << action.title
                   << R"(</a><br>)";
        }
        return result.str();
    }
    string renderHtml(const Response& response)
    {
        ostringstream result;
        result << R"(<!doctype html><html lang="de"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" type="text/css" href="css/style.css">
<link rel="icon" href="data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 100%22><text y=%22.9em%22 font-size=%2290%22>📝</text></svg>">
<title>)";
        result << response.title();
        result << R"(</title>
</head>
<body>
<div class="header">
    <ul>
        <li><div>Todos</div></li>
        <li class="right"><a href="/logout">🚪 <span class="label">Logout</span></a></li>
    </ul>
</div>
<div class="container">
<h2>)";
        result << response.title();
        result << R"(</h2>)";
        result << renderAlert(response.alert());
        result << renderActions(response.actions());
        result << response.content();
        result << R"(</div>
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
</body></html>)";
        return result.str();
    }
};
} // namespace Html