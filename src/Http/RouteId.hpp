#pragma once
#include "Request.hpp"

#include <string>

namespace Http {
using std::string;

struct RouteId {
    RouteId(const Method& method, string path);
    explicit RouteId(string path);
    bool operator<(const RouteId& rhs) const;

private:
    Method m_method = Method::GET;
    string m_path;
};

} // namespace Http
