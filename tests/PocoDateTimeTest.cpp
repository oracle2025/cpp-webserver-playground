#include "doctest.h"

#include <Poco/Data/Date.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/Time.h>

TEST_CASE("Insert and select Poco Date")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql = R"(CREATE TABLE date_test (datecol DATE);)";
    session << sql, now;
    SUBCASE("insert Poco Date")
    {
        Statement insert(session);
        insert << "INSERT INTO date_test (datecol) VALUES (?);",
            bind(Poco::Data::Date(2021, 12, 13));
        CHECK(insert.execute());
        Poco::Data::Date d;
        Statement select(session);
        select << "SELECT datecol FROM date_test;",
            into(d),
            range(0, 1);
        CHECK(select.execute());
        CHECK_EQ(d.year(), 2021);
        CHECK_EQ(d.month(), 12);
        CHECK_EQ(d.day(), 13);
    }
    SUBCASE("Select as string")
    {
        Statement insert(session);
        insert << "INSERT INTO date_test (datecol) VALUES (?);",
            bind(Poco::Data::Date(2021, 12, 13));
        CHECK(insert.execute());
        std::string str;
        Statement select(session);
        select << "SELECT datecol FROM date_test;",
            into(str),
            range(0, 1);
        CHECK(select.execute());
        CHECK_EQ(str, "2021-12-13");
    }
    SUBCASE("insert invalid date")
    {
        Statement insert(session);
        insert << "INSERT INTO date_test (datecol) VALUES ('porky');";
        CHECK(insert.execute());
        Poco::Data::Date d{0, 1, 1};
        Statement select(session);
        select << "SELECT datecol FROM date_test;",
            into(d),
            range(0, 1);
        CHECK(select.execute());
        //Only way to check for invalid data is to check for default date
        CHECK_EQ(d.year(), 0);
        CHECK_EQ(d.month(), 1);
        CHECK_EQ(d.day(), 1);
    }
}