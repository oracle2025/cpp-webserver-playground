#include "Data/Migrations.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "PocoPageHandler.hpp"
#include "doctest.h"
#include "FakeBrowser.hpp"

#include <Poco/Data/SQLite/Connector.h>


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
    auto handler = make_shared<SimpleWebServer>();
    auto login_controller = make_shared<LoginController>(
        make_shared<HelloHandler>("Secret"),
        make_shared<HelloHandler>("Admin"),
        make_shared<HelloHandler>("Public"),
        nullptr)->initialize(handler->router()).shared_from_this();
    handler->defaultHandler(login_controller->getDefaultHandler());
    handler->setPresentation(nullptr);
    handler->finish_init();

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
    auto handler = make_shared<SimpleWebServer>();
    auto login_controller = make_shared<LoginController>(
        make_shared<AlertAndRedirectHandler>(),
        nullptr,
        make_shared<HelloHandler>("Public"),
        nullptr)->initialize(handler->router()).shared_from_this();
    handler->defaultHandler(login_controller->getDefaultHandler());
    handler->setPresentation(nullptr);
    handler->finish_init();

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