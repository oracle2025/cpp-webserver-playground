#pragma once
#include <memory>
#include <string>
#include <map>

using std::shared_ptr;
using std::enable_shared_from_this;
using std::string;
using std::map;

struct Response : public enable_shared_from_this<Response> {

    const int NOT_FOUND = 404;

    static shared_ptr<Response> create();

    Response &content(const string &content);

    Response &code(int code, const string &content);

    Response &cookie(const string &name, const string &value);

    string content() const;

    map<string, string> cookies() const;

    int code() const;

private:
    string m_content;
    map<string, string> m_cookies;
    int m_code = 200;
};

shared_ptr<Response> content(const string &content);
