#pragma once

#include "Http/Session.hpp"
#include "PocoWebServer.hpp"
#include "Trace/trace.hpp"
#include "spdlog/spdlog.h"

#include <iostream>

class PocoPageHandler : public Poco::Net::HTTPRequestHandler {
public:
    using ServerSocket = Poco::Net::ServerSocket;
    using SocketAddress = Poco::Net::SocketAddress;
    using HTTPRequestHandler = Poco::Net::HTTPRequestHandler;
    using HTTPServerRequest = Poco::Net::HTTPServerRequest;
    using HTTPServerResponse = Poco::Net::HTTPServerResponse;
    using HTMLForm = Poco::Net::HTMLForm;
    using NameValueCollection = Poco::Net::NameValueCollection;
    PocoPageHandler(handler_type handler, shared_ptr<Presentation> presentation);

    void handleRequest(
        HTTPServerRequest& request, HTTPServerResponse& response) override;

private:
    handler_type handler;
    shared_ptr<Presentation> m_presentation;
};
