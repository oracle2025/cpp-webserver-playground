
#include "Response.hpp"

#include "doctest.h"

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
    m_code = code;
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
int Response::code() const
{
    return m_code;
}
string Response::mimetype() const
{
    return m_mimetype;
}
shared_ptr<Response> content(const string& content, const string& mimetype)
{
    return Response::create()->content(content, mimetype).shared_from_this();
}

TEST_CASE("Create Response")
{
    auto response = content("Hello World");
}