#pragma once

#include "Record.hpp"
#include "RecordImpl.hpp"

#include <Poco/Tuple.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Poco::Data {
class Session;
}

extern Poco::Data::Session* g_session;

using std::string;

struct SharedTodoDefinition {
    using RecordType = Poco::Tuple<string, string, string, string, bool, string>;
    RecordType data;
    string& id;
    string& m_description;
    string& created_at;
    string& updated_at;
    bool& checked;
    string& user_id;
    explicit SharedTodoDefinition(RecordType  d)
        : data{std::move(d)}
        , id(data.get<0>())
        , m_description(data.get<1>())
        , created_at(data.get<2>())
        , updated_at(data.get<3>())
        , checked(data.get<4>())
        , user_id(data.get<5>())
    {
    }
    SharedTodoDefinition()
        : data{"", "", "", "", false}
        , id(data.get<0>())
        , m_description(data.get<1>())
        , created_at(data.get<2>())
        , updated_at(data.get<3>())
        , checked(data.get<4>())
        , user_id(data.get<5>())
    {
    }
    SharedTodoDefinition(const SharedTodoDefinition& t)
        : data{t.data}
        , id(data.get<0>())
        , m_description(data.get<1>())
        , created_at(data.get<2>())
        , updated_at(data.get<3>())
        , checked(data.get<4>())
        , user_id(data.get<5>())
    {
    }
    SharedTodoDefinition& operator=(const SharedTodoDefinition& other)
    {
        data = other.data;
        return *this;
    }
    string table_name() const;
    vector<ColumnType> columns() const;
    void set(const KeyStringType& key, const string& value);
    string get(const KeyStringType& key) const;
    string description() const;
    static vector<KeyStringType> presentableFields();
    static string presentableName();
};

using SharedTodo = RecordImpl<SharedTodoDefinition>;

