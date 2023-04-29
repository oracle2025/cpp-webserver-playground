#pragma once
#include "Request.hpp"
#include "Response.hpp"
#include "RequestHandler.hpp"

#include <functional>
#include <memory>
#include <variant>

using std::function;
using std::shared_ptr;
using std::variant;
struct RecursiveWebServer : public RequestHandler {
    using handler_type = function<shared_ptr<Response>(const Request& request)>;

    void get(const string& path, handler_type handler)
    {
        router[path] = handler;
    }

    void post(const string& path, handler_type handler)
    {
        get(path, handler);
    }

    void finish_init()
    {
    }

    shared_ptr<Response> handle(const Request& request) override
    {
        if (router.find(request.path()) != router.end()) {
            auto handler = router[request.path()];
            return handler(request);
        }
        return content("404 not found")->code(Response::NOT_FOUND).shared_from_this();
    }

    map<string, handler_type> router;
};
