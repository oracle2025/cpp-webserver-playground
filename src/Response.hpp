#pragma once
#include <map>
#include <memory>
#include <string>

using std::enable_shared_from_this;
using std::map;
using std::shared_ptr;
using std::string;

struct Response : public enable_shared_from_this<Response> {

    static const int NOT_FOUND = 404;
    static const int UNAUTHORIZED = 401;
    static const int HTTP_FOUND = 302;
    static const int OK = 200;

    static shared_ptr<Response> create();

    Response& content(const string& content, const string& mimetype = "text/html");

    Response& code(int code);

    Response& cookie(const string& name, const string& value);

    Response& location(const string& url);

    string content() const;

    string location() const;

    map<string, string> cookies() const;

    int code() const;

    string mimetype() const;

private:
    string m_content;
    map<string, string> m_cookies;
    int m_code = OK;
    string m_mimetype = "text/html";
    string m_location;
};

shared_ptr<Response> content(const string& content, const string& mimetype = "text/html");

shared_ptr<Response> redirect(const string& url);
