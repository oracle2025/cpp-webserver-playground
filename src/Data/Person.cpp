#include "Person.hpp"

#include "String/repeat.hpp"
#include "doctest.h"
#include "FieldTypes.hpp"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>
using Poco::Data::Session;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;

struct Person {
    string& id;
    string& name;
    string& address;
    int& age;
    Person()
        : data{"", "", "", 0}
        , id(data.get<0>())
        , name(data.get<1>())
        , address(data.get<2>())
        , age(data.get<3>())
    {
    }
    Person(const Person& p)
        : data{p.data}
        , id(data.get<0>())
        , name(data.get<1>())
        , address(data.get<2>())
        , age(data.get<3>())
    {
    }
    struct ColumnType {
        string name;
        string type;
    };
    using RecordType = Poco::Tuple<string, string, string, int>;
    string table_name() const
    {
        return "Persons";
    }
    vector<ColumnType> columns() const
    {
        return {
            {"name", "VARCHAR"},
            {"address", "VARCHAR"},
            {"age", "INTEGER(3)"},
        };
    }
    RecordType data;
};

template<typename T>
struct RecordImpl : public T {
    void createTable(Session& session)
    {
        using namespace Poco::Data::Keywords;
        session << "DROP TABLE IF EXISTS " + T::table_name(), now;
        session << createStatement(T::columns()), now;
    }

    string createStatement(const vector<typename T::ColumnType>& columns) const
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

    void insertInto(Session& session)
    {
        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        using String::repeat;
        Statement insert(session);
        insert << "INSERT INTO " + T::table_name() + " VALUES(" + "?, "
                + repeat("?", ", ", T::columns().size()) + ")",
            use(T::data), now;
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
    vector<shared_ptr<RecordImpl<T>>> listAsPointers(Session& session)
    {
        vector<shared_ptr<RecordImpl<T>>> result;
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
    string selectStatement(const vector<typename T::ColumnType>& columns) const
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
    string updateStatement(const vector<typename T::ColumnType>& columns) const
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
    void set(const string& key, const string& value);
    std::map<string, string> values() const;
    string key() const;
    std::vector<string> fields() const;
    HtmlInputType inputType(const string& field) const;

};

TEST_CASE("PersonRecord")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    RecordImpl<Person> person;
    person.createTable(session);
    person.id = "1";
    person.name = "Peter";
    person.address = "1234 London";
    person.age = 23;
    person.insertInto(session);
    auto id = person.id;
    SUBCASE("createStatement")
    {
        CHECK(
            person.createStatement({}) == "CREATE TABLE Persons (id VARCHAR)");
    }
    SUBCASE("Repeat")
    {
        using String::repeat;
        CHECK(repeat("?", ", ", 3) == "?, ?, ?");
    }
    SUBCASE("Pop")
    {
        RecordImpl<Person> record;
        CHECK(record.pop(session, id));
        CHECK(record.name == "Peter");
        CHECK(record.address == "1234 London");
        CHECK(record.age == 23);
    }
    SUBCASE("Update")
    {
        RecordImpl<Person> record;
        record.name = "John";
        record.address = "7777 London";
        record.age = 28;
        CHECK(record.update(session, id));
        CHECK(person.pop(session, id));
        CHECK(person.name == "John");
        CHECK(person.address == "7777 London");
        CHECK(person.age == 28);
    }
    SUBCASE("Erase")
    {
        RecordImpl<Person> record;
        CHECK(record.erase(session, id));
        CHECK(!record.pop(session, id));
    }
    SUBCASE("List")
    {
        RecordImpl<Person> record;
        auto all = record.list(session);
        CHECK(all.size() == 1);
        CHECK(all[0].name == "Peter");
    }
    SUBCASE("ListAsPointers")
    {
        RecordImpl<Person> record;
        auto all = record.listAsPointers(session);
        CHECK(all.size() == 1);
        CHECK(all[0]->name == "Peter");
    }
}