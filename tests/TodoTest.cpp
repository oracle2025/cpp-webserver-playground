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
    using TodoType = RecordImpl<TodoDefinition>;
    TodoType todo = TodoType::RecordType {"0123", "Buy Milk", time_string(), time_string(), 0};
    todo.create_table();
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
        TodoType t;
        t.pop(id);
        CHECK(t.description() == "Buy Milk");
    }
    SUBCASE("update")
    {
        TodoType t;
        t.pop(id);
        t.set("description", "Buy Milk and Eggs");
        t.update();
        {
            TodoType t;
            t.pop(id);
            CHECK(t.description() == "Buy Milk and Eggs");
        }
    }
    SUBCASE("erase")
    {
        TodoType t;
        t.pop(id);
        t.erase();
        CHECK(Todo::list().size() == 0);
    }
    SUBCASE("todoDefintion")
    {
        RecordImpl<TodoDefinition> t;
        t.pop(id);
        CHECK(t.description() == "Buy Milk");
    }
}