#pragma once
#include <map>
#include <string>

using std::map;
using std::string;
namespace Http {

enum class Method { GET, POST, METHOD_NOT_ALLOWED };

class Request {
public:
    Request(
        string path,
        map<string, string> cookies = {},
        map<string, string> parameters = {},
        string query = "",
        Method method = Method::GET,
        string userAgent = "");

    Request() = default;

    bool hasCookie(const string& name) const;

    string cookie(const string& name) const;

    bool hasParameter(const string& name) const;

    string parameter(const string& name) const;

    const map<string, string>& allParameters() const;

    string query() const;

    string path() const;

    string userAgent() const;

    Method method() const;

private:
    map<string, string> m_cookies;
    map<string, string> m_parameters;
    string m_query;
    string m_path;
    Method m_method = Method::GET;
    string m_userAgent;
};
} // namespace Http