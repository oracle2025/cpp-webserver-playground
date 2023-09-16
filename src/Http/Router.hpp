#pragma once

#include "Request.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace Http {

using std::function;
using std::map;
using std::shared_ptr;
using std::string;

struct Request;
struct Response;

using handler_type = function<shared_ptr<Response>(const Request& request)>;

struct RouteId {
    RouteId(const Http::Method& method, string  path)
        : m_method(method)
        , m_path(std::move(path))
    {
    }
    RouteId(string  path)
        : m_path(std::move(path))
    {
    }
    Http::Method m_method = Http::Method::GET;
    string m_path;

    bool operator<(const RouteId& rhs) const
    {
        if (m_method != rhs.m_method) {
            return m_method < rhs.m_method;
        }
        return m_path < rhs.m_path;
    }
};

class Router : public map<RouteId, handler_type> {
public:
    handler_type& findHandlerOrReturnDefault(
        const RouteId& path, handler_type& defaultHandler);

    shared_ptr<Response> handle(const Request& request);

    Router& get(const string& path, handler_type handler);
    Router& post(const string& path, handler_type handler);
};

} // namespace Http