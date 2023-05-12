#pragma once

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Presentation.hpp"

#include <functional>
#include <memory>
#include <variant>

using std::function;
using std::shared_ptr;
using std::variant;
using Html::Presentation;

struct TestServer {
    using request_response_handler_type
        = function<shared_ptr<Response>(const Request& request)>;

    void get(const string& path, request_response_handler_type handler)
    {
        router[path] = handler;
    }

    void post(const string& path, request_response_handler_type handler)
    {
        router[path] = handler;
    }

    void defaultHandler(request_response_handler_type handler)
    {
        m_defaultHandler = std::move(handler);
    }

    void finish_init()
    {
    }
    void setPresentation(shared_ptr<Presentation> presentation)
    {
    }

    shared_ptr<Response> handle(const Request& request);

private:
    map<string, request_response_handler_type> router;
    request_response_handler_type m_defaultHandler;
};
