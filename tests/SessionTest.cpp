#include "Http/Session.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "doctest.h"

using Http::Request;
using Http::Response;
using Http::Session;

TEST_CASE("Session")
{
    Request request;
    Response response;

    Session(request).clearSession();
    CHECK_FALSE(Session(request).isLoggedIn());
    CHECK_THROWS(Session(request).userId());
    SUBCASE("login")
    {
        Data::User user;
        user.id = "123";
        user.username = "porky";
        Session(request).current(response).login(user);
        Request request2{
            "/", {{"session-id", response.cookies()["session-id"]}}};
        CHECK(Session(request2).isLoggedIn());
        CHECK(Session(request2).userId() == "123");
        CHECK(Session(request2).userName() == "porky");
        CHECK_FALSE(Session(request2).isAdmin());

        Request request3;
        CHECK_FALSE(Session(request3).isLoggedIn());
        CHECK_THROWS(Session(request3).userId());
    }
}