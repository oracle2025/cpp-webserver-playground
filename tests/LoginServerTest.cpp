#include "Data/MigrationsV3.hpp"
#include "Data/User.hpp"
#include "Login/LoginController.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

void loginLogout(LoginController<TestServer>& w, map<string, string>& cookieJar)
{
    CHECK(w.handle({"/secret", cookieJar})->content() == "Access denied");
    map<string, string> params;
    params["username"] = "admin";
    params["password"] = "Adm1n!";
    auto response = w.handle({"/login", cookieJar, params});
    cookieJar = response->cookies();
    response = w.handle({"/secret", cookieJar});
    CHECK(response->content() == "Success");
    response = w.handle({"/logout", cookieJar});
    cookieJar = response->cookies();
}

TEST_CASE("Login Server")
{
    using Http::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    LoginController<TestServer> w(nullptr, nullptr, nullptr);
    Session::clearAll();
    SUBCASE("Login")
    {
        CHECK(w.handle({"/secret"})->content() == "Access denied");
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response = w.handle({"/login", {}, params});
        CHECK(
            w.handle({"/secret", response->cookies()})->content() == "Success");
    }
    SUBCASE("Redirect to Login")
    {
        auto response = w.handle({"/secret_page"});
        CHECK(response->alert().message() == "Please login");
        CHECK(response->status() == 302);
        CHECK(response->location() == "/");
    }
    SUBCASE("Login 100 times")
    {
        map<string, string> cookieJar;
        for (int i = 0; i < 100; i++) {
            loginLogout(w, cookieJar);
        }
        CHECK(Session::listAll().size() == 0);
    }
    SUBCASE("Access Denied 100 times")
    {
        map<string, string> cookieJar;
        for (int i = 0; i < 100; i++) {
            CHECK(
                w.handle({"/secret", cookieJar})->content() == "Access denied");
        }
        CHECK(Session::listAll().size() == 0);
    }
    SUBCASE("Redirect to Login only on valid URIs")
    {
    }
    SUBCASE("Reset alert only after display")
    {
    }
}
