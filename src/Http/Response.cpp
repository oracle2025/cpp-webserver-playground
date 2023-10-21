
#include "Response.hpp"

#include <utility>
namespace Http {

using std::make_shared;
shared_ptr<Response> Response::create()
{
    return make_shared<Response>();
}
Response& Response::content(const string& content, const string& mimetype)
{
    m_content = content;
    m_mimetype = mimetype;
    return *this;
}
Response& Response::code(int code)
{
    m_status = code;
    return *this;
}
Response& Response::cookie(const string& name, const string& value)
{
    m_cookies[name] = value;
    return *this;
}
string Response::content() const
{
    return m_content;
}
map<string, string> Response::cookies() const
{
    return m_cookies;
}
int Response::status() const
{
    return m_status;
}
string Response::mimetype() const
{
    return m_mimetype;
}
string Response::location() const
{
    return m_location;
}
Response& Response::location(const string& url)
{
    m_location = url;
    return *this;
}
Response& Response::appendAction(const ActionLink& action)
{
    m_actions.push_back(action);
    return *this;
}
Response& Response::title(const string& title)
{
    m_title = title;
    return *this;
}
vector<ActionLink> Response::actions() const
{
    return m_actions;
}
string Response::title() const
{
    return m_title;
}
Response& Response::alert(const string& alert, Html::AlertType type)
{
    m_alert = Html::Alert(alert, type);
    return *this;
}
const Html::Alert& Response::alert() const
{
    return m_alert;
}
Response& Response::appendNavBarAction(const ActionLink& action)
{
    m_navbarActions.push_back(action);
    return *this;
}
vector<ActionLink> Response::navBarActions() const
{
    return m_navbarActions;
}
bool Response::sendFile() const
{
    return m_sendFile;
}
Response& Response::sendFile(const string& filename)
{
    m_sendFile = true;
    m_filename = filename;
    return *this;
}
string Response::filename() const
{
    return m_filename;
}
Response& Response::mimetype(const string& mimetype)
{
    m_mimetype = mimetype;
    return *this;
}
Response& Response::form(Input::FormPtr form)
{
    m_form = std::move(form);
    return *this;
}
Input::FormPtr Response::form() const
{
    return m_form;
}

shared_ptr<Response> content(const string& content, const string& mimetype)
{
    return Response::create()->content(content, mimetype).shared_from_this();
}
shared_ptr<Response> redirect(const string& url)
{
    return Response::create()
        ->location(url)
        .code(Response::HTTP_FOUND)
        .shared_from_this();
}
} // namespace Http