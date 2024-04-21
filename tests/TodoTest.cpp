#include "doctest.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;
#include "Data/Migrations.hpp"
#include "Data/Todo.hpp"

#include <sstream>

TEST_CASE("todo")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    Todo todo{Todo::RecordType{
        "0123", "Buy Milk", time_string(), time_string(), false, "", ""}};
    todo.insert();
    auto id = todo.key();
    SUBCASE("list")
    {
        auto result = todo.list();
        CHECK(result.size() == 1);
        CHECK(result[0].key() == id);
    }
    SUBCASE("pop")
    {
        Todo t;
        t.pop(id);
        CHECK(t.description() == "Buy Milk");
    }
    SUBCASE("update")
    {
        Todo t;
        t.pop(id);
        t.set("description", "Buy Milk and Eggs");
        t.update();
        {
            Todo t2;
            t2.pop(id);
            CHECK(t2.description() == "Buy Milk and Eggs");
        }
    }
    SUBCASE("erase")
    {
        Todo t;
        t.pop(id);
        t.erase();
        CHECK(t.list().size() == 0);
    }
    SUBCASE("todoDefintion")
    {
        Todo t;
        t.pop(id);
        CHECK(t.description() == "Buy Milk");
    }
    SUBCASE("invalid Key Value")
    {
        Todo t;
        t.pop(id);
        CHECK_THROWS(t.get("No Symbols? or Äf"));
        CHECK_THROWS(t.get("<b>no html</b>"));
    }
}