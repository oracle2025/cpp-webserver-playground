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
        CHECK(w.handle({"/secret", response->cookies()})->content() == "Success");
    }
}
