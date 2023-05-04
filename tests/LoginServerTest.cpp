#include "Login/LoginServer.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

TEST_CASE("Login Server")
{
    LoginServer<TestServer> w(nullptr, nullptr);
    SUBCASE("Login")
    {
        CHECK(w.getPage("/secret") == "Access denied");
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response = w.postTo("/login", params);
        CHECK(w.getPage("/secret", response->cookies()) == "Success");
    }
}
