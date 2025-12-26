#include "Person.hpp"

#include "FieldTypes.hpp"
#include "RecordImpl.hpp"
#include "String/repeat.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>
using Poco::Data::Session;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

struct Person {
    using RecordType = Poco::Tuple<string, string, string, int>;
    RecordType data;
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
    string table_name() const
    {
        return "Persons";
    }
    string presentableName() const
    {
        return "Person";
    }
    vector<ColumnType> columns() const
    {
        static const vector<ColumnType> cols =
         {
            {"name", "VARCHAR", HtmlInputType::TEXT},
            {"address", "VARCHAR", HtmlInputType::TEXT},
            {"age", "INTEGER(3)", HtmlInputType::NUMBER},
        };
        return cols;
    }
    void set(const string& key, const string& value)
    {
        if (key == "name") {
            name = value;
        } else if (key == "address") {
            address = value;
        } else if (key == "age") {
            age = atoi(value.c_str());
        }
    }
    string get(const string& key) const
    {
        if (key == "name") {
            return name;
        } else if (key == "address") {
            return address;
        } else if (key == "age") {
            return std::to_string(age);
        }
        return "";
    }
    string description() const
    {
        return "Person " + name;
    }
    std::vector<KeyStringType> presentableFields() const
    {
        return {"name"};
    }
    void validate()
    {
    }
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
        CHECK(all[0]->values()["name"] == "Peter");
    }
}