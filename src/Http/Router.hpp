#pragma once

#include "Request.hpp"
#include "handler_type.hpp"
#include "RouteId.hpp"
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




class Router : public map<RouteId, handler_type> {
public:
    handler_type& findHandlerOrReturnDefault(
        const RouteId& path, handler_type& defaultHandler);

    shared_ptr<Response> handle(const Request& request);

    Router& get(const string& path, handler_type handler);
    Router& post(const string& path, handler_type handler);
};

} // namespace Http