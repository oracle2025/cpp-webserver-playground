#pragma once

#include "Footer.hpp"
#include "Header.hpp"
#include "Response.hpp"

#include <sstream>
#include <string>

using std::ostringstream;

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
    string renderAlert(const string& alert)
    {
        if (alert.empty()) {
            return "";
        }
        return R"(<div class="alert-danger">⚠️ )" + alert + R"(</div>)";
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
<title>)";
        result << response.title();
        result << R"(</title>
</head>
<body>
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