#include "Data/MigrationsV1.hpp"
#include "Data/User.hpp"
#include "Login/LoginController.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

TEST_CASE("Login Server")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsV1 m;
    //m.perform();

    LoginController<TestServer> w(nullptr, nullptr);
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
    SUBCASE("Redirect to Login only on valid URIs")
    {}
    SUBCASE("Reset alert only after display")
    {}
}
