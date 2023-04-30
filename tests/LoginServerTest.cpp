#include "LoginServer.hpp"

#include "TestServer.hpp"
#include "doctest.h"

TEST_CASE("Login Server")
{
    LoginServer<TestServer> w(nullptr);
    auto actual = w.getPage("/");
    const auto expected
        = R"(<form action="/login" method="post"><input type="text" m_id="username" name="username"><input type="password" m_id="password" name="password"><input type="submit" m_id="submit" name="submit"></form>)";
    CHECK(actual == expected);
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
