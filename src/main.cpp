#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "Page.hpp"
#include <iostream>

using namespace std;
using HTTPRequestHandler = Poco::Net::HTTPRequestHandler;
using HTTPServerRequest = Poco::Net::HTTPServerRequest;
using HTTPServerResponse = Poco::Net::HTTPServerResponse;

class PageHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");
        auto &responseStream = response.send();
        responseStream << Page("Title", "Content").get();
    }
};

class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:

    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &request) {
        return new PageHandler();
    }
};

class TestServer : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string> &args) {
        using ServerSocket = Poco::Net::ServerSocket;
        using HTTPServer = Poco::Net::HTTPServer;
        using HTTPServerParams = Poco::Net::HTTPServerParams;
        ServerSocket socket(8080);
        auto pParams = new HTTPServerParams();
        HTTPServer server(new HandlerFactory(), socket, pParams);
        server.start();
        waitForTerminationRequest();
        server.stop();
        return EXIT_OK;
    }
};

POCO_SERVER_MAIN(TestServer);
