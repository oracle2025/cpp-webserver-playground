#include "Presentation.hpp"

#include "String/escape.hpp"

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
    return R"(<div class="alert )" + alertClass + R"(">⚠️ )" + String::escape(alert.message())
        + R"(</div>)";
}
string Presentation::renderActions(const vector<ActionLink>& actions)
{
    if (actions.empty()) {
        return "";
    }
    ostringstream result;
    for (const auto& action : actions) {
        result << R"(<a href=")" << action.url << R"(" class="create button btn btn-primary">)"
               << action.title << R"(</a><br>)";
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
    for (const auto& action : actions) {
        result << R"(<li class=")" << action.liClass << R"("><a href=")"
               << action.url << R"(">)" << action.title << R"(</a></li>)";
    }
    return result.str();
}
string Presentation::renderHtml(const Response& response)
{
    ostringstream result;
    result << R"(<!DOCTYPE html><html lang="de"><head>
<meta charset="utf-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<!-- Latest compiled and minified CSS -->
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@3.3.7/dist/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">
<!-- Optional theme -->
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@3.3.7/dist/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous">
<!-- <link rel="stylesheet" type="text/css" href="/css/style.css"> -->
<style>
body {
  padding-top: 50px;
}
</style>
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
    <nav class="navbar navbar-inverse navbar-fixed-top">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="/">Todo List</a>
        </div>
        <div id="navbar" class="collapse navbar-collapse">
          <ul class="nav navbar-nav">)";
    result << renderNavBarActions(response.navBarActions());
    result << R"(
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </nav>
<div class="container">
    <div class=row>
        <div class="col-sm-3">
        </div>
        <div class="col-sm-6">
<h2>)";
    result << response.title();
    result << R"(</h2>)";
    result << renderAlert(response.alert());
    result << renderActions(response.actions());
    result << response.content();
    result << R"(
        </div>
        <div class="col-sm-3">
        </div>
    </div>
</div>
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
</body></html>)";
    return result.str();
}
} // namespace Html
