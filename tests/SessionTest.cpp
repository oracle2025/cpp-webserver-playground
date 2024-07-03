#include "Data/Migrations.hpp"
#include "Data/SessionRow.hpp"
#include "Http/Session.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

using Http::Request;
using Http::Response;
using Http::Session;

TEST_CASE("Session")
{
    Poco::Data::SQLite::Connector::registerConnector();
    using Data::SessionRow;

    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();
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

TEST_CASE("Session Database")
{
    Poco::Data::SQLite::Connector::registerConnector();
    using Data::SessionRow;
    using Poco::Data::Session;

    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    SessionRow row{SessionRow::RecordType{
        "123",
        true,
        "456",
        "porky",
        "2020-01-01",
        "2020-01-02",
        "/path",
        "user-agent",
        true,
        "alert",
        "alert-type"}};
    row.insert();
    auto id = row.key();
    SUBCASE("list")
    {
        auto result = row.list();
        CHECK(result.size() == 1);
        CHECK(result[0].key() == id);
    }
    SUBCASE("pop")
    {
        SessionRow r;
        r.pop(id);
        CHECK(
            r.description().find("porky 2020-01-01 2020-01-02")
            != std::string::npos);
    }
    SUBCASE("update")
    {
        SessionRow r;
        r.pop(id);
        r.set("alert", "Buy Milk and Eggs");
        r.update();
        {
            SessionRow r2;
            r2.pop(id);
            CHECK(
                r2.get("alert").find("Buy Milk and Eggs") != std::string::npos);
        }
    }
    SUBCASE("erase")
    {
        SessionRow r;
        r.pop(id);
        r.erase();
        CHECK(r.list().size() == 0);
    }
    SUBCASE("convert SessionData")
    {
        Http::SessionData data{
            generateRandomSessionId(), "/path", "user-agent"};
        data.alert(Html::Alert{"alert", Html::AlertType::DANGER});
        const auto createdAt = data.createdAt();
        const auto lastUsedAt = data.lastUsedAt();
        SessionRow r;
        r.fromSessionData(data);
        r.insert();
        const auto key = r.key();
        SessionRow r2;
        r2.pop(key);
        auto data2 = r2.toSessionData();
        CHECK(data2.path() == data.path());
        CHECK(data2.userAgent() == data.userAgent());
        CHECK(data2.createdAt() == createdAt);
        CHECK(data2.lastUsedAt() == lastUsedAt);
        CHECK(data2.hasAlert());
        CHECK(data2.getAlert().message() == "alert");
        CHECK(data2.getAlert().alertType() == Html::AlertType::DANGER);
    }
}