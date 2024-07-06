#include "Presentation.hpp"

#include "String/escape.hpp"
#include "Template/BaseTemplate.hpp"

#include <nlohmann/json.hpp>

namespace Html {

string Presentation::render(const Response& response)
{
    if (response.noPresentation()) {
        return response.content();
    }
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
    return R"(<div class="alert )" + alertClass + R"(">⚠️ )"
        + String::escape(alert.message()) + R"(</div>)";
}
string Presentation::renderActions(const vector<ActionLink>& actions)
{
    if (actions.empty()) {
        return "";
    }
    ostringstream result;
    for (const auto& action : actions) {
        result << R"(<a href=")" << action.url
               << R"(" class="create button btn btn-primary">)" << action.title
               << R"(</a><br>)";
    }
    result << R"(<br>)";
    return result.str();
}
string Presentation::renderNavBarActions(const vector<ActionLink>& actions)
{
    if (actions.empty()) {
        return "";
    }
    ostringstream result;
    result << R"(<ul class="nav navbar-nav">)" << "\n";
    for (const auto& action : actions) {
        if (action.liClass != "right") {
            result << R"(<li class=")" << action.liClass << R"("><a href=")"
                   << action.url << R"(">)" << action.title << R"(</a></li>)";
        }
    }
    result << R"(</ul>)" << "\n";
    result << R"(<ul class="nav navbar-nav navbar-right">)" << "\n";
    for (const auto& action : actions) {
        if (action.liClass == "right") {
            result << R"(<li class=")" << action.liClass << R"("><a href=")"
                   << action.url << R"(">)" << action.title << R"(</a></li>)";
        }
    }
    result << R"(</ul>)" << "\n";
    return result.str();
}

string Presentation::renderHtml(const Response& response)
{
    auto data = nlohmann::json::object();
    data["title"] = response.title();
    data["navBarActions"] = renderNavBarActions(response.navBarActions());
    data["alerts"] = renderAlert(response.alert());
    data["actions"] = renderActions(response.actions());
    data["content"] = response.content();
    data["applicationName"] = m_applicationName;
    return Template::BaseTemplate(TEMPLATE_DIR "/index.html").render(data);
}
Presentation::Presentation(const string& applicationName)
    : m_applicationName(applicationName)
{
}
} // namespace Html
