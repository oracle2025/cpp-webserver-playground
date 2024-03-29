#pragma once

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
    PocoPageHandler(
        handler_type handler, shared_ptr<Presentation> presentation);
    static map<string, string> convert(const NameValueCollection& cookies);
    static Http::Method httpMethod(const string& requestMethod);
    static void addCookiesToResponse(
        HTTPServerResponse& response, const Response& result);

    void handleRequest(
        HTTPServerRequest& request, HTTPServerResponse& response) override;
    Input::FormPtr form() const;

private:
    handler_type handler;
    shared_ptr<Presentation> m_presentation;
    Input::FormPtr m_form;
};
