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
    TodoDefinition& operator=(const TodoDefinition &other)
    {
        data = other.data;
        return *this;
    }
    string table_name() const;
    vector<ColumnType> columns() const;
    void set(const string& key, const string& value);
    string get(const string& key) const;
    string description() const;
};

using Todo = RecordImpl<TodoDefinition>;

string time_string();