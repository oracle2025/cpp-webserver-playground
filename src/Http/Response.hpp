#pragma once
#include "ActionLink.hpp"
#include "Alert.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

using std::enable_shared_from_this;
using std::map;
using std::shared_ptr;
using std::string;
using std::vector;

struct Response : public enable_shared_from_this<Response> {

    static const int NOT_FOUND = 404;
    static const int UNAUTHORIZED = 401;
    static const int HTTP_FOUND = 302;
    static const int OK = 200;

    static shared_ptr<Response> create();

    Response& content(
        const string& content, const string& mimetype = "text/html");

    Response& code(int code);

    Response& cookie(const string& name, const string& value);

    Response& location(const string& url);

    Response& title(const string& title);

    Response& appendAction(const ActionLink& action);

    Response& appendNavBarAction(const ActionLink& action);

    Response& alert(const string& alert, Html::AlertType type = Html::AlertType::DANGER);

    string content() const;

    string location() const;

    map<string, string> cookies() const;

    int code() const;

    string mimetype() const;

    vector<ActionLink> actions() const;

    vector<ActionLink> navBarActions() const;

    string title() const;

    const Html::Alert& alert() const;

private:
    string m_content;
    map<string, string> m_cookies;
    int m_code = OK;
    string m_mimetype = "text/html";
    string m_location;
    vector<ActionLink> m_actions;
    vector<ActionLink> m_navbarActions;
    string m_title;
    Html::Alert m_alert;
};

shared_ptr<Response> content(
    const string& content, const string& mimetype = "text/html");

shared_ptr<Response> redirect(const string& url);
