#include "Login/LoginComponent.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

TEST_CASE("Login Server")
{
    LoginComponent<TestServer> w(nullptr, nullptr);
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
