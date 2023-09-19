#include "PocoWebServer.hpp"

#include "Http/Session.hpp"
#include "PocoPageHandler.hpp"
#include "Trace/trace.hpp"
#include "spdlog/spdlog.h"

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
    ServerSocket socket(SocketAddress("localhost", SERVER_PORT));
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
            auto uri = Poco::URI(request.getURI());
            auto method = convertPocoHttpMethod(request.getMethod());
            return new PocoPageHandler(
                router.findHandlerOrReturnDefault(
                    {method, uri.getPath()}, m_defaultHandler),
                m_presentation);
        }

        Http::Method convertPocoHttpMethod(const string& method)
        {
            const map<string, Http::Method> methods
                = {{Poco::Net::HTTPRequest::HTTP_GET, Http::Method::GET},
                   {Poco::Net::HTTPRequest::HTTP_POST, Http::Method::POST}};
            if (methods.find(method) == methods.end()) {
                throw std::runtime_error("Unknown HTTP method");
            }
            return methods.at(method);
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
