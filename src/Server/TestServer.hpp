#pragma once

#include "Http/Request.hpp"
#include "Http/Response.hpp"

#include <functional>
#include <memory>
#include <variant>

using std::function;
using std::shared_ptr;
using std::variant;

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

    string getPage(
        const string& path,
        const map<string, string>& cookies = {},
        const map<string, string>& params = {});
    shared_ptr<Response> getResponse(
        const string& path,
        const map<string, string>& cookies = {},
        const map<string, string>& params = {});

    shared_ptr<Response> postTo(
        const string& path, const map<string, string>& params);

private:
    map<string, request_response_handler_type> router;
    request_response_handler_type m_defaultHandler;
};
