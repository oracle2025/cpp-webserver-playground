#include "Request.hpp"
namespace Http {

Request::Request(
    string path,
    map<string, string> cookies,
    map<string, string> parameters,
    string query,
    Method method,
    string userAgent)
    : m_cookies(std::move(cookies))
    , m_parameters(std::move(parameters))
    , m_query(std::move(query))
    , m_path(std::move(path))
    , m_method(method)
    , m_userAgent(std::move(userAgent))
{
}
bool Request::hasCookie(const string& name) const
{
    return m_cookies.find(name) != m_cookies.end();
}
string Request::cookie(const string& name) const
{
    return m_cookies.at(name);
}
bool Request::hasParameter(const string& name) const
{
    return m_parameters.find(name) != m_parameters.end();
}
string Request::parameter(const string& name) const
{
    return m_parameters.at(name);
}
const map<string, string>& Request::allParameters() const
{
    return m_parameters;
}
string Request::query() const
{
    return m_query;
}
string Request::path() const
{
    return m_path;
}
Method Request::method() const
{
    return m_method;
}
string Request::userAgent() const
{
    return m_userAgent;
}

} // namespace Http