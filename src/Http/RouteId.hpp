#pragma once
#include <string>
#include "Request.hpp"

namespace Http {
using std::string;

struct RouteId {
    RouteId(const Http::Method& method, string  path);
    explicit RouteId(string  path);
    Http::Method m_method = Http::Method::GET;
    string m_path;

    bool operator<(const RouteId& rhs) const;
};

} // Http

