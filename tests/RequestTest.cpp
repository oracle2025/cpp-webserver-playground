#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "doctest.h"

TEST_CASE("Request Session")
{
    Http::Request request;
    Http::Response response;
    Http::Session session(request);

    session.isLoggedIn();
    session.clearSession();
    session.createSession(response);
}