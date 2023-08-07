#include "Data/MigrationsV2.hpp"
#include "Filter/ByOwner.hpp"
#include "Login/LoginController.hpp"
#include "Server/CrudController.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

using Poco::Data::Session;
using std::map;
using std::string;

void loginAs(
    RequestHandler& w, const string& username, map<string, string>& cookieJar)
{
    map<string, string> params;
    params["username"] = username;
    params["password"] = "Adm1n!";
    auto r = w.handle({"/login", cookieJar, params});
    cookieJar = r->cookies();
    // CHECK(r->content() == "Success");
}
void addItem(
    RequestHandler& w,
    const string& description,
    map<string, string>& cookieJar)
{
    map<string, string> params;
    params["description"] = description;
    auto r = w.handle({"/item/create", cookieJar, params});
    //    CHECK(r->content() == "Success");
}
string extractUUID(const string& content)
{
    auto uuid_found = content.find("name=\"");
    uuid_found += string("name=\"").size();
    auto substr = content.substr(uuid_found);
    auto uuid_end = substr.find("\"");
    auto uuid = substr.substr(0, uuid_end);
    return uuid;
}
TEST_CASE("By Owner")
{
    LoginController<TestServer> w(
        make_shared<CrudController<TestServer, Filter::ByOwner>>("/item"),
        nullptr,
        nullptr);

    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Todo t;

    CHECK_EQ(
        t.createStatement(t.columns()),
        "CREATE TABLE Todo (id VARCHAR, description VARCHAR, created_at "
        "VARCHAR, updated_at VARCHAR, checked INTEGER(3), user_id VARCHAR)");

    Data::MigrationsV2 m;
    m.perform();

    User user;
    user.username = "alice";
    user.setPassword("Adm1n!");
    user.insert();
    user.username = "bob";
    user.setPassword("Adm1n!");
    user.insert();
    SUBCASE("List Items")
    {

        map<string, string> cookieJar;

        loginAs(w, "alice", cookieJar);
        addItem(w, "Buy Milk", cookieJar);
        CHECK_FALSE(
            w.handle({"/item/", cookieJar})->content().find("Buy Milk")
            == string::npos);

        loginAs(w, "bob", cookieJar);
        // ASSERT_EQ(w.list().size(), 0);
        addItem(w, "Buy Eggs", cookieJar);
        addItem(w, "Buy Bread", cookieJar);
        loginAs(w, "alice", cookieJar);
        // ASSERT_EQ(w.list().size(), 1);
        // ASSERT_EQ(w.list()[0], "Buy Milk");
        CHECK_FALSE(
            w.handle({"/item/", cookieJar})->content().find("Buy Milk")
            == string::npos);
        CHECK(
            w.handle({"/item/", cookieJar})->content().find("Buy Eggs")
            == string::npos);
        CHECK(
            w.handle({"/item/", cookieJar})->content().find("Buy Bread")
            == string::npos);
    }
    SUBCASE("Edit Items")
    {
        // /edit?uuid-1234-5678-9012-3456 -> Form
        // /edit?uuid-1234-5678-9012-3456 -> Access Denied or 404
        map<string, string> cookieJar;

        loginAs(w, "alice", cookieJar);
        addItem(w, "Buy Milk", cookieJar);

        auto response = w.handle({"/item/", cookieJar});
        CHECK_FALSE(response->content().find("Buy Milk") == string::npos);

        auto uuid = extractUUID(response->content());

        response = w.handle({"/item/edit", cookieJar, {}, uuid});
        CHECK_EQ(response->status(), 200);

        loginAs(w, "bob", cookieJar);
        response = w.handle({"/item/edit", cookieJar, {}, uuid});
        CHECK_EQ(response->status(), 404);
    }
    SUBCASE("Delete Items")
    {
        map<string, string> cookieJar;

        loginAs(w, "alice", cookieJar);
        addItem(w, "Buy Milk", cookieJar);

        auto response = w.handle({"/item/", cookieJar});
        CHECK_FALSE(response->content().find("Buy Milk") == string::npos);

        auto uuid = extractUUID(response->content());

        loginAs(w, "bob", cookieJar);

        response = w.handle({"/item/delete", cookieJar, {}, uuid});
        CHECK_EQ(response->status(), 404);

        loginAs(w, "alice", cookieJar);
        response = w.handle({"/item/edit", cookieJar, {}, uuid});
        CHECK_EQ(response->status(), 200);

        response = w.handle({"/item/delete", cookieJar, {{"confirmed", "true"}}, uuid});
        CHECK_EQ(response->status(), 302);
        response = w.handle({"/item/edit", cookieJar, {}, uuid});
        CHECK_EQ(response->status(), 404);
    }
    SUBCASE("Update Items")
    {
        map<string, string> cookieJar;

        loginAs(w, "alice", cookieJar);
        addItem(w, "Buy Milk", cookieJar);

        auto response = w.handle({"/item/", cookieJar});
        CHECK_FALSE(response->content().find("Buy Milk") == string::npos);

        auto uuid = extractUUID(response->content());

        loginAs(w, "bob", cookieJar);

        response = w.handle({"/item/update", cookieJar, {{"description", "Buy Cream"}}, uuid});
        CHECK_EQ(response->status(), 404);

        loginAs(w, "alice", cookieJar);
        response = w.handle({"/item/update", cookieJar, {{"description", "Buy Cream"}}, uuid});
        CHECK_EQ(response->status(), 302);

        response = w.handle({"/item/", cookieJar});
        CHECK(response->content().find("Buy Milk") == string::npos);
        CHECK_FALSE(response->content().find("Buy Cream") == string::npos);
    }
}