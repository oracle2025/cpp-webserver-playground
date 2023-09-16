#include "PocoPageHandler.hpp"
#include "doctest.h"
struct FakeHTTPServerParams : public Poco::Net::HTTPServerParams {
    ~FakeHTTPServerParams() override = default;
};
struct FakeHTTPServerRequest : public PocoPageHandler::HTTPServerRequest {

    FakeHTTPServerRequest(Poco::Net::HTTPServerResponse& response)
        : m_response(response)
    {
    }
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
        return m_params;
    }

    Poco::Net::HTTPServerResponse& response() const override
    {
        return m_response;
    }

    bool secure() const override
    {
        return false;
    }

private:
    std::stringstream m_stream;
    Poco::Net::SocketAddress m_clientAddress;
    Poco::Net::SocketAddress m_serverAddress;
    Poco::Net::HTTPServerResponse& m_response;
    FakeHTTPServerParams m_params;
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
    FakeHTTPServerResponse response;
    FakeHTTPServerRequest request(response);
    PocoPageHandler handler(
        [](const Request& request) { return Http::content("Hello World"); },
        nullptr);

    request.setMethod(Poco::Net::HTTPRequest::HTTP_PUT);

    CHECK(request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT);

    handler.handleRequest(request, response);
}
