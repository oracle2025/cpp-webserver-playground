#pragma once
#include <map>
#include <string>

using std::map;
using std::string;
namespace Http {

struct Request {
    Request(
        string path,
        map<string, string> cookies = {},
        map<string, string> parameters = {},
        string query = "");

    Request() = default;

    bool hasCookie(const string& name) const;

    string cookie(const string& name) const;

    bool hasParameter(const string& name) const;

    string parameter(const string& name) const;

    const map<string, string>& allParameters() const;

    string query() const;

    string path() const;

private:
    map<string, string> m_cookies;
    map<string, string> m_parameters;
    string m_query;
    string m_path;
};
} // namespace Http