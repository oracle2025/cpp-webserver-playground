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

class WebServer {
public:
    virtual ~WebServer() = default;
    virtual Http::Router& router() = 0;
    virtual void defaultHandler(Http::handler_type handler) = 0;
    virtual void finish_init() = 0;
    virtual void setPresentation(
        std::shared_ptr<Html::Presentation> presentation)
        = 0;
};
