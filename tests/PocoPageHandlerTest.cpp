#include "PocoPageHandler.hpp"
#include "doctest.h"

struct FakeHTTPServerRequest : public PocoPageHandler::HTTPServerRequest {
    std::istream& stream() override
    {
        return m_stream;
    }

    const Poco::Net::SocketAddress& clientAddress() const override
    {
        return m_clientAddress;
    }

    const Poco::Net::SocketAddress& serverAddress() const override
    {
        return m_serverAddress;
    }

    const Poco::Net::HTTPServerParams& serverParams() const override
    {
    }

    Poco::Net::HTTPServerResponse& response() const override
    {
    }

    bool secure() const override
    {
        return false;
    }

private:
    std::stringstream m_stream;
    Poco::Net::SocketAddress m_clientAddress;
    Poco::Net::SocketAddress m_serverAddress;
};

struct FakeHTTPServerResponse : public PocoPageHandler::HTTPServerResponse {
    void sendContinue()
    {
    }
    std::ostream& send()
    {
        return m_stream;
    }
    void sendFile(const std::string& path, const std::string& mediaType)
    {
    }
    void sendBuffer(const void* pBuffer, std::size_t length)
    {
    }
    void redirect(const std::string& uri, HTTPStatus status = HTTP_FOUND)
    {
    }
    void requireAuthentication(const std::string& realm)
    {
    }
    bool sent() const
    {
        return false;
    }

private:
    std::stringstream m_stream;
};
TEST_CASE("PocoPageHandler Test")
{
    FakeHTTPServerRequest request;
    FakeHTTPServerResponse response;
    PocoPageHandler handler(
        [](const Request& request) { return Http::content("Hello World"); },
        nullptr);

    request.setMethod(Poco::Net::HTTPRequest::HTTP_PUT);

    CHECK(request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT);

    handler.handleRequest(request, response);
}
