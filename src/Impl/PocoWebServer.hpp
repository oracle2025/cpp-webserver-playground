#pragma once

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Presentation.hpp"

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/URI.h>

#include <functional>
#include <memory>
#include <utility>
#include <variant>
using Html::Presentation;
using std::function;
using std::shared_ptr;
using std::variant;

class PocoWebServer {
public:
    using request_response_handler_type
        = function<shared_ptr<Response>(const Request& request)>;

    using HTTPServer = Poco::Net::HTTPServer;

    void get(const string& path, request_response_handler_type handler)
    {
        router[path] = handler;
    }

    void post(const string& path, request_response_handler_type handler)
    {
        router[path] = handler;
    }

    void finish_init();

    void start()
    {
        server->start();
    }

    void stop()
    {
        server->stop();
    }
    void defaultHandler(request_response_handler_type handler)
    {
        m_defaultHandler = std::move(handler);
    }
    void setPresentation(shared_ptr<Presentation> presentation)
    {
        m_presentation = std::move(presentation);
    }

private:
    map<string, request_response_handler_type> router;
    shared_ptr<HTTPServer> server;
    shared_ptr<Presentation> m_presentation;
    request_response_handler_type m_defaultHandler;
};
