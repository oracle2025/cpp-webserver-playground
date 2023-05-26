#include "PocoWebServer.hpp"
#include "Http/Session.hpp"

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
            Router& router,
            handler_type& defaultHandler,
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
                    handler_type handler, shared_ptr<Presentation> presentation)
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
                    const Request req(
                        uri.getPath(), cookiesMap, parameters, uri.getQuery());
                    auto result = handler(req);
                    Http::Session::addAlertToSession(req, *result);
                    for (auto& [key, value] : result->cookies()) {
                        HTTPCookie cookie(key, value);
                        cookie.setSameSite(HTTPCookie::SAME_SITE_STRICT);
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
                    auto& responseStream = response.send();
                    responseStream << m_presentation->render(*result);
                }

            private:
                handler_type handler;
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
            return new PageHandler(
                router.findHandlerOrReturnDefault(
                    uri.getPath(), m_defaultHandler),
                m_presentation);
        }

    private:
        Router& router;
        handler_type& m_defaultHandler;
        shared_ptr<Presentation> m_presentation;
    };
    server = make_shared<HTTPServer>(
        new HandlerFactory(m_router, m_defaultHandler, m_presentation),
        socket,
        pParams);
}
void PocoWebServer::start()
{
    server->start();
}
void PocoWebServer::stop()
{
    server->stop();
}
void PocoWebServer::defaultHandler(handler_type handler)
{
    m_defaultHandler = std::move(handler);
}
void PocoWebServer::setPresentation(shared_ptr<Presentation> presentation)
{
    m_presentation = std::move(presentation);
}
Router& PocoWebServer::router()
{
    return m_router;
}
