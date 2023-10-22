#include "Data/MigrationsV3.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "PocoPageHandler.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

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

    void setStreamValue(const string& value)
    {
        m_stream.str(value);
    }

private:
    std::istringstream m_stream;
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
        setStatus(status);
        set("Location", uri);
    }
    void requireAuthentication(const std::string& realm)
    {
    }
    bool sent() const
    {
        return false;
    }
    string result() const
    {
        return m_stream.str();
    }
    void reset()
    {
        setStatus(HTTP_OK);
        clear();
        m_stream.str("");
    }

private:
    std::ostringstream m_stream;
};
class HelloHandler : public RequestHandler {
public:
    explicit HelloHandler(string message)
        : m_message(std::move(message))
    {
    }
    shared_ptr<Response> handle(const Request& request) override
    {
        return Http::content(m_message);
    }

private:
    string m_message;
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
TEST_CASE("Login Logout Cookies Test")
{

    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    /*
     * 1 Login fail -> no cookie
     * 2 Sending Invented Session ID -> no cookie, no Session
     * 3 Login success -> cookie
     * 4 Sending existing Session ID -> no cookie value in response
     * 5 Logout -> delete cookie
     */
    using std::make_shared;
    auto handler = make_shared<LoginController<SimpleWebServer>>(
        make_shared<HelloHandler>("Secret"),
        make_shared<HelloHandler>("Admin"),
        make_shared<HelloHandler>("Public"),
        nullptr);

    PocoPageHandler pageHandler(
        [handler](const Request& request) { return handler->handle(request); },
        nullptr);
    FakeHTTPServerResponse response;
    FakeHTTPServerRequest request(response);

    request.setURI("http://localhost:8080/login");
    request.setMethod(Poco::Net::HTTPRequest::HTTP_POST);
    request.setStreamValue("username=admin&password=Adm1n!");

    pageHandler.handleRequest(request, response);

    std::vector<Poco::Net::HTTPCookie> cookies;
    response.getCookies(cookies);
    CHECK(cookies.size() == 1);
    // CHECK(response.result() == "");
}
struct Form {
    map<string, string>& fields();
};
struct FakeBrowser {
    FakeBrowser(PocoPageHandler& handler)
        : m_handler(handler)
    {
    }
    void location(
        const string& l,
        const string& method = Poco::Net::HTTPRequest::HTTP_GET,
        const string& streamValue = "")
    {
        FakeHTTPServerResponse response;
        FakeHTTPServerRequest request(response);
        request.setURI(l);
        request.setMethod(method);
        request.setCookies(m_cookieJar);
        Poco::URI uri(l);
        m_scheme = uri.getScheme();
        m_host = uri.getHost();
        m_location = l;
        request.setStreamValue(streamValue);
        m_handler.handleRequest(request, response);
        std::vector<Poco::Net::HTTPCookie> cookies;
        response.getCookies(cookies);
        for (auto& cookie : cookies) {
            m_cookieJar.set(cookie.getName(), cookie.getValue());
        }
        while (response.getStatus()
               == Poco::Net::HTTPResponse::HTTPStatus::HTTP_FOUND) {
            m_location = uri.getScheme() + "://" + uri.getHost()
                + response["Location"];
            response.reset();
            request.setURI(m_location);
            request.setCookies(m_cookieJar);
            // look at response headers
            request.setMethod(Poco::Net::HTTPRequest::HTTP_GET);
            request.setStreamValue("");
            m_handler.handleRequest(request, response);
        }
        m_form = m_handler.form();
        m_pageContents = response.result();
    }
    string location() const
    {
        return m_location;
    }
    Input::FormPtr form()
    {
        return m_form;
    }
    string pageContents()
    {
        return m_pageContents;
    }
    void submit()
    {
        auto l = m_scheme + "://" + m_host + form()->action();
        location(l, Poco::Net::HTTPRequest::HTTP_POST, form()->data());
    }

private:
    PocoPageHandler& m_handler;
    Poco::Net::NameValueCollection m_cookieJar;
    string m_pageContents;
    Input::FormPtr m_form;
    string m_scheme;
    string m_host;
    string m_location;
};
TEST_CASE("Render Alert after Redirect")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;
    Data::MigrationsLatest m;
    m.perform();
    using std::make_shared;
    class AlertAndRedirectHandler : public RequestHandler {
    public:
        explicit AlertAndRedirectHandler()
        {
        }
        shared_ptr<Response> handle(const Request& request) override
        {
            if (request.path() == "/alert") {
                return Http::redirect("/")
                    ->alert("Hello from Alert", Html::AlertType::INFO)
                    .shared_from_this();
            }
            return Http::content("");
        }

    private:
    };
    auto handler = make_shared<LoginController<SimpleWebServer>>(
        make_shared<AlertAndRedirectHandler>(),
        nullptr,
        make_shared<HelloHandler>("Public"),
        nullptr);

    PocoPageHandler pageHandler(
        [handler](const Request& request) { return handler->handle(request); },
        nullptr);

    FakeBrowser browser(pageHandler);
    browser.location("http://localhost:8080/login");

    CHECK(browser.form().get() != nullptr);

    browser.form()->set("username", "admin");
    browser.form()->set("password", "Adm1n!");

    browser.submit();

    CHECK(
        browser.pageContents().find("Logged in successfully") != string::npos);

    browser.location("http://localhost:8080/alert");

    CHECK(browser.pageContents().find("Hello from Alert") != string::npos);

}