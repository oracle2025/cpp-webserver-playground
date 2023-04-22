#pragma once
#include <map>
#include <memory>
#include <string>

using std::enable_shared_from_this;
using std::map;
using std::shared_ptr;
using std::string;

struct Response : public enable_shared_from_this<Response> {

    const int NOT_FOUND = 404;

    static shared_ptr<Response> create();

    Response& content(const string& content, const string& mimetype = "text/html");

    Response& code(int code, const string& content);

    Response& cookie(const string& name, const string& value);

    string content() const;

    map<string, string> cookies() const;

    int code() const;

    string mimetype() const;

private:
    string m_content;
    map<string, string> m_cookies;
    int m_code = 200;
    string m_mimetype = "text/html";
};

shared_ptr<Response> content(const string& content, const string& mimetype = "text/html");
