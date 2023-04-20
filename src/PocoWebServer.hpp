#pragma once

#include "Response.hpp"
#include "Request.hpp"

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/URI.h>

#include <functional>
#include <memory>
#include <variant>
using std::variant;
using std::shared_ptr;
using std::function;


class PocoWebServer {
public:
    using response_handler_type = function<shared_ptr<Response>()>;
    using request_response_handler_type = function<shared_ptr<Response>(const Request &request)>;
    using handlers_type = variant<response_handler_type, request_response_handler_type>;

    using HTTPServer = Poco::Net::HTTPServer;

    void get(const string &path, request_response_handler_type handler) {
        router[path] = handler;
    }

    void get(const string &path, response_handler_type handler) {
        router[path] = handler;
    }

    void post(const string &path, response_handler_type handler) {
        router[path] = handler;
    }

    void post(const string &path, request_response_handler_type handler) {
        router[path] = handler;
    }

    void finish_init();

    void start() { server->start(); }

    void stop() { server->stop(); }

private:
    map<string, handlers_type> router;
    shared_ptr<HTTPServer> server;
};

