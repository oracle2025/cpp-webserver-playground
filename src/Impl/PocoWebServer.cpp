#include "PocoWebServer.hpp"

#include <iostream>

using std::make_shared;

void PocoWebServer::finish_init()
{
    using ServerSocket = Poco::Net::ServerSocket;
    using SocketAddress = Poco::Net::SocketAddress;
    using HTTPServerParams = Poco::Net::HTTPServerParams;
    using HTTPRequestHandlerFactory = Poco::Net::HTTPRequestHandlerFactory;
    using HTTPRequestHandler = Poco::Net::HTTPRequestHandler;
    using HTTPServerRequest = Poco::Net::HTTPServerRequest;
    using HTTPServerResponse = Poco::Net::HTTPServerResponse;
    using HTMLForm = Poco::Net::HTMLForm;
    using NameValueCollection = Poco::Net::NameValueCollection;
    ServerSocket socket(SocketAddress("localhost", 8080));
    auto pParams = new HTTPServerParams();
    class HandlerFactory : public HTTPRequestHandlerFactory {
    public:
        HandlerFactory(
            map<string, request_response_handler_type>& router,
            request_response_handler_type& defaultHandler,
            shared_ptr<Presentation> presentation)
            : router(router)
            , m_defaultHandler(defaultHandler)
            , m_presentation(presentation)
        {
        }

        HTTPRequestHandler* createRequestHandler(
            const HTTPServerRequest& request)
        {
            class PageHandler : public HTTPRequestHandler {
            public:
                PageHandler(
                    request_response_handler_type handler,
                    shared_ptr<Presentation> presentation)
                    : handler(handler)
                    , m_presentation(presentation)
                {
                }

                void handleRequest(
                    HTTPServerRequest& request,
                    HTTPServerResponse& response) override
                {
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("text/html");
                    using HTTPCookie = Poco::Net::HTTPCookie;
                    NameValueCollection cookies;
                    request.getCookies(cookies);
                    Poco::URI uri(request.getURI());
                    std::cout << "Request: " << uri.toString() << std::endl;
                    HTMLForm form(request, request.stream());
                    map<string, string> parameters;
                    map<string, string> cookiesMap;
                    for (auto& [key, value] : cookies) {
                        cookiesMap[key] = value;
                    }
                    for (auto& [key, value] : form) {
                        parameters[key] = value;
                    }
                    auto result = handler(Request(
                        uri.getPath(), cookiesMap, parameters, uri.getQuery()));
                    for (auto& [key, value] : result->cookies()) {
                        HTTPCookie cookie(key, value);
                        if (value.empty()) {
                            cookie.setMaxAge(0);
                        }
                        response.addCookie(cookie);
                    }
                    if (!result->alert().empty()) {
                        response.addCookie({"alert", result->alert()});
                    } else {
                        HTTPCookie cookie("alert", "");
                        cookie.setMaxAge(0);
                        response.addCookie(cookie);
                    }
                    response.setContentType(result->mimetype());
                    if (result->code() == Response::HTTP_FOUND
                        && (!result->location().empty())) {
                        response.redirect(result->location());
                    }
                    auto& responseStream = response.send();
                    if (cookiesMap.count("alert") > 0) {
                        result->alert(cookiesMap["alert"]);
                    }
                    responseStream << m_presentation->render(*result);
                }

            private:
                request_response_handler_type handler;
                shared_ptr<Presentation> m_presentation;
            };
            struct EmptyHandler : public HTTPRequestHandler {
                void handleRequest(
                    HTTPServerRequest& request,
                    HTTPServerResponse& response) override
                {
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("text/html");
                    auto& responseStream = response.send();
                    responseStream << "Not found!";
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                }
            };
            auto uri = Poco::URI(request.getURI());
            if (router.find(uri.getPath()) != router.end()) {
                return new PageHandler(router[uri.getPath()], m_presentation);
            }
            return new PageHandler(m_defaultHandler, m_presentation);
        }

    private:
        map<string, request_response_handler_type>& router;
        request_response_handler_type& m_defaultHandler;
        shared_ptr<Presentation> m_presentation;
    };
    server = make_shared<HTTPServer>(
        new HandlerFactory(router, m_defaultHandler, m_presentation),
        socket,
        pParams);
}
