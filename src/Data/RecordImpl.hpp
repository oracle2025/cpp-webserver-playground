#pragma once
#include "Data/FieldTypes.hpp"
#include "Data/Record.hpp"
#include "String/createRandomUUID.hpp"
#include "String/repeat.hpp"

#include <Poco/Data/Session.h>

using Poco::Data::Session;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

extern Poco::Data::Session* g_session;

struct ColumnType {
    string name;
    string type;
    HtmlInputType inputType;
};

template<typename T>
struct RecordImpl : public T, public Record {
    RecordImpl() = default;
    RecordImpl(const RecordImpl<T>&) = default;
    RecordImpl(const typename T::RecordType& data)
        : T(data)
    {
    }
    void create_table()
    {
        createTable(*g_session);
    }
    void createTable(Session& session)
    {
        using namespace Poco::Data::Keywords;
        session << "DROP TABLE IF EXISTS " + T::table_name(), now;
        session << createStatement(T::columns()), now;
    }

    string createStatement(const vector<ColumnType>& columns) const
    {
        string statement = "CREATE TABLE " + T::table_name() + " (";
        statement += "id VARCHAR, ";
        for (auto& column : columns) {
            statement += column.name + " " + column.type + ", ";
        }
        statement = statement.substr(0, statement.length() - 2);
        statement += ")";
        return statement;
    }

    void insert()
    {
        insertInto(*g_session);
    }
    void insertInto(Session& session)
    {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        using String::repeat;
        T::id = String::createRandomUUID();
        Statement insert(session);
        insert << "INSERT INTO " + T::table_name() + " VALUES(" + "?, "
                + repeat("?", ", ", T::columns().size()) + ")",
            use(T::data), now;
    }
    bool pop(const string& _id)
    {
        return pop(*g_session, _id);
    }
    bool pop(Session& session, const string& _id)
    {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        using String::repeat;
        T::id = _id;
        Statement select(session);
        typename T::RecordType record;
        select << "SELECT * FROM " + T::table_name() + " WHERE id = ?",
            use(T::id), into(T::data), range(0, 1);
        if (select.execute() == 0) {
            return false;
        }
        return true;
    }
    vector<RecordImpl<T>> list()
    {
        return list(*g_session);
    }
    vector<RecordImpl<T>> list(Session& session)
    {
        vector<RecordImpl<T>> result;
        using Poco::Data::Statement;
        using namespace Poco::Data::Keywords;
        RecordImpl<T> record;
        Statement select(session);
        select << selectStatement(T::columns()), into(record.data), range(0, 1);
        while (!select.done()) {
            if (select.execute()) {
                result.push_back(record);
            }
        }
        return result;
    }
    vector<shared_ptr<Record>> listAsPointers()
    {
        return listAsPointers(*g_session);
    }
    vector<shared_ptr<Record>> listAsPointers(Session& session)
    {
        vector<shared_ptr<Record>> result;
        using Poco::Data::Statement;
        using namespace Poco::Data::Keywords;
        RecordImpl<T> record;
        Statement select(session);
        select << selectStatement(T::columns()), into(record.data), range(0, 1);
        while (!select.done()) {
            if (select.execute()) {
                result.push_back(make_shared<RecordImpl<T>>(record));
            }
        }
        return result;
    }
    string selectStatement(const vector<ColumnType>& columns) const
    {
        string statement = "SELECT ";
        statement += "id, ";
        for (auto& column : columns) {
            statement += column.name + ", ";
        }
        statement = statement.substr(0, statement.length() - 2);
        statement += " FROM " + T::table_name();
        return statement;
    }
    bool update()
    {
        return update(*g_session, key());
    }
    bool update(const string& _id)
    {
        return update(*g_session, _id);
    }
    bool update(Session& session, const string& _id)
    {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        using String::repeat;
        T::id = _id;
        Statement update(session);
        update << updateStatement(T::columns()), use(T::data), use(T::id);
        if (update.execute() == 0) {
            return false;
        }
        return true;
    }
    string updateStatement(const vector<ColumnType>& columns) const
    {
        string statement = "UPDATE " + T::table_name() + " SET ";
        statement += "id = ?, ";
        for (auto& column : columns) {
            statement += column.name + " = ?, ";
        }
        statement = statement.substr(0, statement.length() - 2);
        statement += " WHERE id = ?";
        return statement;
    }
    bool erase()
    {
        return erase(*g_session, key());
    }
    bool erase(const string& _id)
    {
        return erase(*g_session, _id);
    }
    bool erase(Session& session, const string& _id)
    {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        T::id = _id;
        Statement erase(session);
        erase << "DELETE FROM " + T::table_name() + " WHERE id = ?", use(T::id);
        if (erase.execute() == 0) {
            return false;
        }
        return true;
    }

    string key() const override
    {
        return T::id;
    }
    std::vector<string> fields() const override
    {
        std::vector<string> result;
        for (auto& column : T::columns()) {
            result.push_back(column.name);
        }
        return result;
    }
    HtmlInputType inputType(const string& field) const override
    {
        for (auto& column : T::columns()) {
            if (column.name == field) {
                return column.inputType;
            }
        }
        return HtmlInputType::TEXT;
    }
    std::map<string, string> values() const override
    {
        std::map<string, string> result;
        for (auto& column : T::columns()) {
            result[column.name] = T::get(column.name);
        }
        return result;
    }
};
