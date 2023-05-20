#pragma once

#include "Record.hpp"
#include "RecordImpl.hpp"

#include <Poco/Tuple.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Poco::Data {
class Session;
}

extern Poco::Data::Session* g_session;

using std::string;

struct TodoDefinition {
    using RecordType = Poco::Tuple<string, string, string, string, bool>;
    RecordType data;
    string& id;
    string& m_description;
    string& created_at;
    string& updated_at;
    bool& checked;
    TodoDefinition(const RecordType &d)
        : data{d}
        , id(data.get<0>())
        , m_description(data.get<1>())
        , created_at(data.get<2>())
        , updated_at(data.get<3>())
        , checked(data.get<4>())
    {
    }    TodoDefinition()
        : data{"", "", "", "", false}
        , id(data.get<0>())
        , m_description(data.get<1>())
        , created_at(data.get<2>())
        , updated_at(data.get<3>())
        , checked(data.get<4>())
    {
    }
    TodoDefinition(const TodoDefinition& t)
        : data{t.data}
        , id(data.get<0>())
        , m_description(data.get<1>())
        , created_at(data.get<2>())
        , updated_at(data.get<3>())
        , checked(data.get<4>())
    {
    }
    string table_name() const
    {
        return "Todo";
    }
    vector<ColumnType> columns() const
    {
        return {
            {"description", "VARCHAR", HtmlInputType::TEXT},
            {"created_at", "VARCHAR", HtmlInputType::HIDDEN},
            {"updated_at", "VARCHAR", HtmlInputType::HIDDEN},
            {"checked", "INTEGER(3)", HtmlInputType::CHECKBOX},
        };
    }
    void set(const string& key, const string& value)
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
    string get(const string& key) const
    {
        if (key == "id") {
            return id;
        } else if (key == "description") {
            return m_description;
        } else if (key == "created_at") {
            return created_at;
        } else if (key == "updated_at") {
            return updated_at;
        } else if (key == "checked") {
            return checked ? "yes" : "no";
        }
        return "";
    }
    string description() const
    {
        return m_description;
    }
};

struct Todo : public Record {
    Todo() = default;

    Todo(
        string id,
        string description,
        string created_at,
        string updated_at,
        bool checked);

    string key() const override;

    bool checked = false;

    static std::vector<Todo> list();

    static std::vector<std::shared_ptr<Record>> listAsPointers();

    void insert();

    void update();

    bool pop(const string& _id);

    void erase();

    void set(const string& key, const string& value);

    static void create_table();

    std::vector<string> fields() const override;
    HtmlInputType inputType(const string& field) const override;

    std::map<string, string> values() const override;

    string description() const;

private:
    string id;
    string m_description;
    string created_at;
    string updated_at;
};

using TTodo = Todo;

string time_string();