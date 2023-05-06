#include "doctest.h"
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;
#include "Data/Todo.hpp"

TEST_CASE("todo")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;
    Todo::create_table();
    Todo todo = {"0123", "Buy Milk", time_string(), time_string(), 0};
    todo.insert();
    auto id = todo.id;
    SUBCASE("list")
    {
        auto result = Todo::list();
        CHECK(result.size() == 1);
        CHECK(result[0].id == id);
    }
    SUBCASE("pop")
    {
        Todo t;
        t.pop(id);
        CHECK(t.description == "Buy Milk");
    }
}