#include "Todo.hpp"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/UUIDGenerator.h>

#include <chrono>
#include <iostream>
#include <numeric>
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


std::vector<Todo> Todo::list()
{
    std::vector<Todo> result;
    Todo todo;
    Statement select(*g_session);
    select << "SELECT id, description, created_at, updated_at, checked FROM "
              "Todo",
        into(todo.id), into(todo.m_description), into(todo.created_at),
        into(todo.updated_at), into(todo.checked),
        range(0, 1); //  iterate over result set one row at a time
    while (!select.done()) {
        if (select.execute()) {
            result.push_back(todo);
        }
    }
    return result;
}
std::vector<std::shared_ptr<Record>> Todo::listAsPointers()
{
    std::vector<std::shared_ptr<Record>> result;
    Todo todo;
    Statement select(*g_session);
    select << "SELECT id, description, created_at, updated_at, checked FROM "
              "Todo",
        into(todo.id), into(todo.m_description), into(todo.created_at),
        into(todo.updated_at), into(todo.checked),
        range(0, 1); //  iterate over result set one row at a time
    while (!select.done()) {
        if (select.execute()) {
            result.push_back(std::make_shared<Todo>(todo));
        }
    }
    return result;
}

void Todo::insert()
{
    Statement insert(*g_session);
    id = Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
    created_at = time_string();
    updated_at = time_string();
    typedef Poco::Tuple<string, string, string, string, int> TodoTuple;
    TodoTuple todo_tuple(id, m_description, created_at, updated_at, checked);
    insert << "INSERT INTO Todo VALUES(?, ?, ?, ?, ?)", use(todo_tuple), now;
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
              " id = ?;",
        use(m_description), use(updated_at), use(checked), use(id);
    update.execute();
}

bool Todo::pop(const string& _id)
{
    id = _id;
    Statement select(*g_session);
    select << "SELECT description, "
              "created_at, updated_at, "
              "checked FROM Todo "
              "WHERE id = ?",
        use(id), into(m_description), into(created_at), into(updated_at),
        into(checked),
        range(0, 1); //  iterate over result set one row at a time
    if (select.execute() == 0) {
        return false;
    }
    return true;
}

void Todo::erase()
{
    Statement update(*g_session);
    updated_at = time_string();
    update << "DELETE FROM Todo"
              " WHERE"
              " id = ?;",
        use(id);
    update.execute();
}

void Todo::create_table()
{
    auto& session = *g_session;
    session << "DROP TABLE IF EXISTS Todo", now;
    session << "CREATE TABLE "
               "Todo (id VARCHAR, "
               "description VARCHAR, "
               "created_at VARCHAR, "
               "updated_at VARCHAR, "
               "checked INTEGER(3))",
        now;
}

void Todo::set(const string& key, const string& value)
{
    if (key == "id") {
        id = value;
    } else if (key == "description") {
        m_description = value;
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
        {"id", id},
        {"description", m_description},
        {"created_at", created_at},
        {"updated_at", updated_at},
        {"checked", (checked ? "yes" : "no")},
    };
}
Todo::Todo(
    string id,
    string description,
    string created_at,
    string updated_at,
    bool checked)
    : id(std::move(id))
    , m_description(std::move(description))
    , created_at(std::move(created_at))
    , updated_at(std::move(updated_at))
    , checked(checked)
{
}
string Todo::key() const
{
    return id;
}
std::vector<string> Todo::fields() const
{
    return {"id", "description", "created_at", "updated_at", "checked"};
}
HtmlInputType Todo::inputType(const string& field) const
{
    if (field == "checked") {
        return HtmlInputType::CHECKBOX;
    } else if (field == "id") {
        return HtmlInputType::HIDDEN;
    } else if (field == "created_at") {
        return HtmlInputType::HIDDEN;
    } else if (field == "updated_at") {
        return HtmlInputType::HIDDEN;
    }
    return HtmlInputType::TEXT;
}
string Todo::description() const
{
    return m_description;
}
