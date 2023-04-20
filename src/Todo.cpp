#include "Todo.hpp"

#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/UUIDGenerator.h>

#include <chrono>
#include <iostream>
#include <vector>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

using namespace std;

Session* g_session;

string time_string()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    return std::ctime(&t_c);
}

TEST_CASE("todo")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;
    Todo::create_table();
    Todo todo = {"0123", "Buy Milk", time_string(), time_string(), 0};
    todo.insert();
    auto id = todo.m_id;
    SUBCASE("list")
    {
        auto result = Todo::list();
        CHECK(result.size() == 1);
        CHECK(result[0].m_id == id);
    }
    SUBCASE("pop")
    {
        Todo t;
        t.pop(id);
        CHECK(t.description == "Buy Milk");
    }
}

std::vector<Todo> Todo::list()
{
    std::vector<Todo> result;
    Todo todo;
    Statement select(*g_session);
    select << "SELECT m_id, description, created_at, updated_at, checked FROM "
              "Todo",
        into(todo.m_id), into(todo.description), into(todo.created_at),
        into(todo.updated_at), into(todo.checked),
        range(0, 1); //  iterate over result set one row at a time
    while (!select.done()) {
        select.execute();
        result.push_back(todo);
    }
    return result;
}
std::vector<std::shared_ptr<Record>> Todo::listAsPointers()
{
    std::vector<std::shared_ptr<Record>> result;
    Todo todo;
    Statement select(*g_session);
    select << "SELECT m_id, description, created_at, updated_at, checked FROM "
              "Todo",
        into(todo.m_id), into(todo.description), into(todo.created_at),
        into(todo.updated_at), into(todo.checked),
        range(0, 1); //  iterate over result set one row at a time
    while (!select.done()) {
        select.execute();
        result.push_back(std::make_shared<Todo>(todo));
    }
    return result;
}

void Todo::insert()
{
    Statement insert(*g_session);
    m_id = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
    created_at = time_string();
    updated_at = time_string();
    insert << "INSERT INTO Todo VALUES(?, ?, ?, ?, ?)", use(m_id),
        use(description), use(created_at), use(updated_at), use(checked);

    insert.execute();
}

void Todo::update()
{
    Statement update(*g_session);
    updated_at = time_string();
    update << "UPDATE Todo"
              " SET description = ?,"
              " updated_at = ?,"
              " checked = ?"
              " WHERE"
              " m_id = ?;",
        use(description), use(updated_at), use(checked), use(m_id);
    update.execute();
}

void Todo::pop(const string& _id)
{
    m_id = _id;
    Statement select(*g_session);
    select << "SELECT description, "
              "created_at, updated_at, "
              "checked FROM Todo "
              "WHERE m_id = ?",
        use(m_id), into(description), into(created_at), into(updated_at),
        into(checked),
        range(0, 1); //  iterate over result set one row at a time
    if (!select.done()) {
        select.execute();
    }
}

void Todo::erase()
{
}

void Todo::create_table()
{
    auto& session = *g_session;
    session << "DROP TABLE IF EXISTS Todo", now;
    session << "CREATE TABLE "
               "Todo (m_id VARCHAR, "
               "description VARCHAR, "
               "created_at VARCHAR, "
               "updated_at VARCHAR, "
               "checked INTEGER(3))",
        now;
}

void Todo::set(const string& key, const string& value)
{
    if (key == "m_id") {
        m_id = value;
    } else if (key == "description") {
        description = value;
    } else if (key == "created_at") {
        created_at = value;
    } else if (key == "updated_at") {
        updated_at = value;
    } else if (key == "checked") {
        value == "yes" ? checked = 1 : checked = 0;
    }
}

std::map<string, string> Todo::values() const
{
    return {
        {"m_id", m_id},
        {"description", description},
        {"created_at", created_at},
        {"updated_at", updated_at},
        {"checked", (checked ? "yes" : "no")},
    };
}
