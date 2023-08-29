#include "Presentation.hpp"

#include "String/escape.hpp"

namespace Html {

string Presentation::render(const Response& response)
{
    if (response.mimetype() == "text/html") {
        return renderHtml(response);
    }
    return response.content();
}
string Presentation::renderAlert(const Alert& alert)
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
    return R"(<div class=")" + alertClass + R"(">⚠️ )" + String::escape(alert.message())
        + R"(</div>)";
}
string Presentation::renderActions(const vector<ActionLink>& actions)
{
    if (actions.empty()) {
        return "";
    }
    ostringstream result;
    for (const auto& action : actions) {
        result << R"(<a href=")" << action.url << R"(" class="create button">)"
               << action.title << R"(</a><br>)";
    }
    return result.str();
}
string Presentation::renderNavBarActions(const vector<ActionLink>& actions)
{
    if (actions.empty()) {
        return "";
    }
    ostringstream result;
    for (const auto& action : actions) {
        result << R"(<li class=")" << action.liClass << R"("><a href=")"
               << action.url << R"(">)" << action.title << R"(</a></li>)";
    }
    return result.str();
}
string Presentation::renderHtml(const Response& response)
{
    ostringstream result;
    result << R"(<!doctype html><html lang="de"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" type="text/css" href="/css/style.css">
<link rel="icon" href="data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 100%22><text y=%22.9em%22 font-size=%2290%22>📝</text></svg>">
<script>
function submitForm(elem) {
        elem.form.submit();
}
</script>
<title>)";
    result << response.title();
    result << R"(</title>
</head>
<body>
<div class="header">
    <ul>
        <li><div>Todos</div></li>)";
    result << renderNavBarActions(response.navBarActions());
    result << R"(
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
} // namespace Html
