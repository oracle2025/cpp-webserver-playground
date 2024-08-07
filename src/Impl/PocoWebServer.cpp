#include "PocoWebServer.hpp"

#include <utility>

#include "PocoPageHandler.hpp"
#include "spdlog/spdlog.h"


using std::make_shared;

void PocoWebServer::finish_init()
{
    using ServerSocket = Poco::Net::ServerSocket;
    using SocketAddress = Poco::Net::SocketAddress;
    using HTTPServerParams = Poco::Net::HTTPServerParams;
    using HTTPRequestHandlerFactory = Poco::Net::HTTPRequestHandlerFactory;
    using HTTPRequestHandler = Poco::Net::HTTPRequestHandler;
    using HTTPServerRequest = Poco::Net::HTTPServerRequest;
    ServerSocket socket(SocketAddress("0.0.0.0", m_serverPort));
    auto pParams = new HTTPServerParams();
    class HandlerFactory : public HTTPRequestHandlerFactory {
    public:
        HandlerFactory(
            Router& router,
            handler_type& defaultHandler,
            shared_ptr<Presentation> presentation)
            : router(router)
            , m_defaultHandler(defaultHandler)
            , m_presentation(std::move(presentation))
        {
        }

        HTTPRequestHandler* createRequestHandler(
            const HTTPServerRequest& request) override
        {
            auto uri = Poco::URI(request.getURI());
            auto method = convertPocoHttpMethod(request.getMethod());
            return new PocoPageHandler(
                router.findHandlerOrReturnDefault(
                    {method, uri.getPath()}, m_defaultHandler),
                m_presentation);
        }

        static Http::Method convertPocoHttpMethod(const string& method)
        {
            const map<string, Http::Method> methods
                = {{Poco::Net::HTTPRequest::HTTP_GET, Http::Method::GET},
                   {Poco::Net::HTTPRequest::HTTP_POST, Http::Method::POST}};
            if (methods.find(method) == methods.end()) {
                TRACE_THROW("Unknown HTTP method");
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
void PocoWebServer::setServerPort(int port)
{
    m_serverPort = port;
}
