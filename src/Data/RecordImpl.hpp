#pragma once
#include "Data/FieldTypes.hpp"
#include "Data/Record.hpp"
#include "String/createRandomUUID.hpp"
#include "String/repeat.hpp"
#include "Trace/trace.hpp"
#include "spdlog/spdlog.h"

#include <Poco/Data/Session.h>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

extern Poco::Data::Session* g_session;

struct ColumnType {
    KeyStringType name;
    string type;
    HtmlInputType inputType;
};

namespace Http {
class Request;
}

template<typename T>
struct RecordImpl : public T, public RecordExtended {
    using Session = Poco::Data::Session;
    RecordImpl() = default;
    explicit RecordImpl(const Http::Request& request) { };
    RecordImpl(const RecordImpl<T>&) = default;
    explicit RecordImpl(const typename T::RecordType& data)
        : T(data)
    {
    }
    void create_table()
    {
        createTable(*g_session);
    }
    void createTable(Session& session)
    try {
        using namespace Poco::Data::Keywords;
        session << "DROP TABLE IF EXISTS " + T::table_name(), now;
        session << createStatement(T::columns()), now;
    } catch (...) {
        TRACE_RETHROW("Could not create table");
    }
    string presentableName() const override
    {
        return T::table_name();
    }
    void setImpl(const KeyStringType& key, const string& value) override
    {
        T::set(key, value);
    }
    string getImpl(const KeyStringType& key) const override
    {
        return T::get(key);
    }

    string createStatement(const vector<ColumnType>& columns) const
    try {
        string statement = "CREATE TABLE " + T::table_name() + " (";
        statement += "id VARCHAR, ";
        for (auto& column : columns) {
            statement += column.name + " " + column.type + ", ";
        }
        statement = statement.substr(0, statement.length() - 2);
        statement += ")";
        return statement;
    } catch (...) {
        TRACE_RETHROW("Could not create statement");
    }

    void insert() override
    {
        insertInto(*g_session);
    }
    void insert(const string& id)
    {
        insertInto(id, *g_session);
    }
    void insertInto(Session& session)
    {
        insertInto(String::createRandomUUID(), session);
    }
    void insertInto(const string& id, Session& session)
    {
        T::validate();
        try {
            using namespace Poco::Data::Keywords;
            using Poco::Data::Statement;
            using String::repeat;
            T::id = id;
            Statement insert(session);
            const string sql = "INSERT INTO " + T::table_name() + " ("
                + orderedColumnNames(T::columns()) + ")VALUES(" + "?, "
                + repeat("?", ", ", T::columns().size()) + ")";
            spdlog::debug("SQL: {}", sql);
            insert << sql, use(T::data), now;
        } catch (...) {
            TRACE_RETHROW("Could not insert");
        }
    }
    bool pop(const string& _id) override
    {
        return pop(*g_session, _id);
    }
    bool pop(Session& session, const string& _id)
    try {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        using String::repeat;
        T::id = _id;
        Statement select(session);
        typename T::RecordType record;
        select << "SELECT " + orderedColumnNames(T::columns()) + " FROM "
                + T::table_name() + " WHERE id = ?",
            use(T::id), into(T::data), range(0, 1);
        if (select.execute() == 0) {
            return false;
        }
        return true;
    } catch (...) {
        TRACE_RETHROW("Could not pop");
    }
    void reset()
    {
        T::data = {};
        T::id = "";
    }
    vector<RecordImpl<T>> list()
    {
        return list(*g_session);
    }
    vector<RecordImpl<T>> list(Session& session)
    try {
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
    } catch (...) {
        TRACE_RETHROW("Could not select");
    }
    vector<shared_ptr<Record>> listAsPointers() override
    {
        return listAsPointers(*g_session);
    }
    vector<shared_ptr<Record>> listAsPointers(Session& session)
    try {
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
    } catch (...) {
        TRACE_RETHROW("Could not list");
    }
    string selectStatement(const vector<ColumnType>& columns) const
    try {
        string statement = "SELECT ";
        statement += orderedColumnNames(columns);
        statement += " FROM " + T::table_name();
        return statement;
    } catch (...) {
        TRACE_RETHROW("Could not create statement");
    }
    string orderedColumnNames(const vector<ColumnType>& columns) const
    {
        string result = "id, ";
        for (auto& column : columns) {
            result += column.name + ", ";
        }
        result = result.substr(0, result.length() - 2);
        return result;
    }
    bool update() override
    {
        return update(*g_session, key());
    }
    bool update(const string& _id)
    {
        return update(*g_session, _id);
    }
    bool update(Session& session, const string& _id)
    try {
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
    } catch (...) {
        TRACE_RETHROW("Could not update");
    }
    string updateStatement(const vector<ColumnType>& columns) const
    try {
        string statement = "UPDATE " + T::table_name() + " SET ";
        statement += "id = ?, ";
        for (auto& column : columns) {
            statement += column.name + " = ?, ";
        }
        statement = statement.substr(0, statement.length() - 2);
        statement += " WHERE id = ?";
        return statement;
    } catch (...) {
        TRACE_RETHROW("Could not create statement");
    }
    bool erase() override
    {
        return erase(*g_session, key());
    }
    bool erase(const string& _id)
    {
        return erase(*g_session, _id);
    }
    string descriptionImpl() const override
    {
        return T::description();
    }
    std::vector<KeyStringType> presentableFieldsImpl() const override
    {
        return T::presentableFields();
    }

    bool erase(Session& session, const string& _id)
    try {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        T::id = _id;
        Statement erase(session);
        erase << "DELETE FROM " + T::table_name() + " WHERE id = ?", use(T::id);
        if (erase.execute() == 0) {
            return false;
        }
        return true;
    } catch (...) {
        TRACE_RETHROW("Could not delete");
    }

    string key() const override
    {
        return T::id;
    }
    std::vector<KeyStringType> fields() const override
    {
        std::vector<KeyStringType> result;
        for (auto& column : T::columns()) {
            result.push_back(column.name);
        }
        return result;
    }
    HtmlInputType inputType(const KeyStringType& field) const override
    {
        for (auto& column : T::columns()) {
            if (column.name == field) {
                return column.inputType;
            }
        }
        return HtmlInputType::TEXT;
    }
    std::map<KeyStringType, string> values() const override
    {
        std::map<KeyStringType, string> result;
        for (auto& column : T::columns()) {
            result[column.name] = T::get(column.name);
        }
        return result;
    }
};
