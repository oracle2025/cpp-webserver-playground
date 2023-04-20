#pragma once

#include "Request.hpp"
#include "Response.hpp"

#include <functional>
#include <memory>
#include <variant>

using std::function;
using std::shared_ptr;
using std::variant;

struct TestServer {
    using response_handler_type = function<shared_ptr<Response>()>;
    using request_response_handler_type
        = function<shared_ptr<Response>(const Request& request)>;
    using handlers_type
        = variant<response_handler_type, request_response_handler_type>;

    void get(const string& path, response_handler_type handler)
    {
        router[path] = handler;
    }

    void get(const string& path, request_response_handler_type handler)
    {
        router[path] = handler;
    }

    void post(const string& path, response_handler_type handler)
    {
        router[path] = handler;
    }

    void post(const string& path, request_response_handler_type handler)
    {
        router[path] = handler;
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
    map<string, handlers_type> router;
};
