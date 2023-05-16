#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/RecordSet.h>
using Poco::Data::Session;
#include "Data/Todo.hpp"
#include <sstream>

TEST_CASE("todo")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;
    Todo::create_table();
    Todo todo = {"0123", "Buy Milk", time_string(), time_string(), 0};
    todo.insert();
    auto id = todo.key();
    SUBCASE("list")
    {
        auto result = Todo::list();
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
            Todo t;
            t.pop(id);
            CHECK(t.description() == "Buy Milk and Eggs");
        }
    }
    SUBCASE("erase")
    {
        Todo t;
        t.pop(id);
        t.erase();
        CHECK(Todo::list().size() == 0);
    }
}