#pragma once

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
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
using Http::handler_type;
using Http::Request;
using Http::Response;
using Http::Router;

using std::function;
using std::shared_ptr;
using std::variant;

class PocoWebServer  {
public:
    using HTTPServer = Poco::Net::HTTPServer;

    void finish_init() ;

    void start();

    void stop();
    void defaultHandler(handler_type handler) ;
    void setPresentation(shared_ptr<Presentation> presentation) ;
    Router& router() ;

private:
    Router m_router;
    shared_ptr<HTTPServer> server;
    shared_ptr<Presentation> m_presentation;
    handler_type m_defaultHandler;
};
