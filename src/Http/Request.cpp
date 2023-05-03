#include "Request.hpp"
Request::Request(
    string path,
    map<string, string> cookies,
    map<string, string> parameters,
    string query)
    : m_cookies(move(cookies))
    , m_parameters(move(parameters))
    , m_query(move(query))
    , m_path(move(path))
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
