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
    PocoPageHandler(handler_type handler, shared_ptr<Presentation> presentation)
        : handler(handler)
        , m_presentation(presentation)
    {
    }

    void handleRequest(
        HTTPServerRequest& request, HTTPServerResponse& response) override
    {
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        using HTTPCookie = Poco::Net::HTTPCookie;
        NameValueCollection cookies;
        request.getCookies(cookies);
        Poco::URI uri(request.getURI());
        spdlog::info("Request: {}", uri.toString());
        HTMLForm form(request, request.stream());
        map<string, string> parameters;
        map<string, string> cookiesMap;
        for (auto& [key, value] : cookies) {
            cookiesMap[key] = value;
        }
        for (auto& [key, value] : form) {
            parameters[key] = value;
        }
        const Request req(
            uri.getPath(), cookiesMap, parameters, uri.getQuery());
        shared_ptr<Response> result;
        try {
            result = handler(req);
        } catch (...) {
            std::ostringstream str;
            Trace::backtrace(std::current_exception(), str);
            spdlog::error("Exception: {}", str.str());
            using HTTPResponse = Poco::Net::HTTPResponse;
            response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.send() << "Internal Server Error\n";
            return;
        }
        Http::Session::addAlertToSession(req, *result);
        for (auto& [key, value] : result->cookies()) {
            HTTPCookie cookie(key, value);
            cookie.setSameSite(HTTPCookie::SAME_SITE_STRICT);
            cookie.setPath("/");
            if (value.empty()) {
                cookie.setMaxAge(0);
            }
            response.addCookie(cookie);
        }
        response.setContentType(result->mimetype());
        if (result->status() == Response::HTTP_FOUND
            && (!result->location().empty())) {
            response.redirect(result->location());
        }
        if (result->sendFile()) {
            response.sendFile(result->filename(), result->mimetype());
        } else {
            auto& responseStream = response.send();
            responseStream << m_presentation->render(*result);
        }
    }

private:
    handler_type handler;
    shared_ptr<Presentation> m_presentation;
};
