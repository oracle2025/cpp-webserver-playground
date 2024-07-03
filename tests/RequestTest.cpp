#include "Data/Migrations.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

TEST_CASE("Request Session")
{
    Poco::Data::SQLite::Connector::registerConnector();

    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();
    Http::Request request;
    Http::Response response;
    Http::Session s(request);

    s.isLoggedIn();
    s.clearSession();
    s.createSession(response);
}