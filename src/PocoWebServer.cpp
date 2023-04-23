#include "PocoWebServer.hpp"

#include "overloaded.hpp"

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
            request_response_handler_type& defaultHandler)
            : router(router)
            , m_defaultHandler(defaultHandler)
        {
        }

        HTTPRequestHandler* createRequestHandler(
            const HTTPServerRequest& request)
        {
            class PageHandler : public HTTPRequestHandler {
            public:
                PageHandler(request_response_handler_type handler)
                    : handler(handler)
                {
                }

                void handleRequest(
                    HTTPServerRequest& request,
                    HTTPServerResponse& response) override
                {
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("text/html");
                    using HTTPCookie = Poco::Net::HTTPCookie;
                    /*HTTPCookie cookie("session-m_id",
                                      Poco::UUIDGenerator::defaultGenerator().createRandom().toString());
                    response.addCookie(cookie);*/
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
                        uri.toString(),
                        cookiesMap,
                        parameters,
                        uri.getQuery()));
                    for (auto& [key, value] : result->cookies()) {
                        HTTPCookie cookie(key, value);
                        if (value.empty()) {
                            cookie.setMaxAge(0);
                        }
                        response.addCookie(cookie);
                    }
                    response.setContentType(result->mimetype());
                    auto& responseStream = response.send();
                    responseStream << result->content();
                }

            private:
                request_response_handler_type handler;
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
                return new PageHandler(router[uri.getPath()]);
            }
            return new PageHandler(m_defaultHandler);
        }

    private:
        map<string, request_response_handler_type>& router;
        request_response_handler_type& m_defaultHandler;
    };
    server = make_shared<HTTPServer>(
        new HandlerFactory(router, m_defaultHandler), socket, pParams);
}
