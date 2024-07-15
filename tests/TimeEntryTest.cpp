#include "Data/Migrations.hpp"
#include "String/currentDateTime.hpp"
#include "TimeRecordingApp/TimeEntry.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

static void insert_for_day(
    TimeEntry& t,
    const std::string& day,
    const std::string& start,
    const std::string& stop)
{
    t.set("event_date", day);
    t.set("event_time", start);
    t.set("event_type", "start");
    t.insert();
    t.set("event_date", day);
    t.set("event_time", stop);
    t.set("event_type", "stop");
    t.insert();
}
static void insert_for_start(
    TimeEntry& t, const std::string& day, const std::string& start)
{
    t.set("event_date", day);
    t.set("event_time", start);
    t.set("event_type", "start");
    t.insert();
}
static void insert_for_stop(
    TimeEntry& t, const std::string& day, const std::string& stop)
{
    t.set("event_date", day);
    t.set("event_time", stop);
    t.set("event_type", "stop");
    t.insert();
}

TEST_CASE("TimeEntry")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;
    Data::MigrationsLatest m;
    m.perform();
    SUBCASE(" is empty for user")
    {
        TimeEntry ted;
        CHECK(ted.isEmptyFor("user_id"));
    }
    SUBCASE(" has yearsFor")
    {
        TimeEntry t1;
        t1.set("employee_id", "user_id_123");
        t1.set("event_date", "2021-01-01");
        t1.set("event_type", "start");
        t1.insert();
        TimeEntry t2;
        t2.set("employee_id", "user_id_123");
        t2.set("event_date", "2022-01-02");
        t2.set("event_type", "start");
        t2.insert();
        const auto yearsFor = t1.yearsFor("user_id_123");
        CHECK_EQ(yearsFor.size(), 2);
        CHECK_EQ(yearsFor, std::vector<int>{2021, 2022});
    }
    SUBCASE("has monthsFor")
    {
        TimeEntry t1;
        t1.set("employee_id", "user_id_123");
        t1.set("event_date", "2021-01-01");
        t1.set("event_type", "start");
        t1.insert();
        TimeEntry t2;
        t2.set("employee_id", "user_id_123");
        t2.set("event_date", "2021-02-02");
        t2.set("event_type", "start");
        t2.insert();
        const auto monthsFor = t1.monthsFor("user_id_123", 2021);
        CHECK_EQ(monthsFor.size(), 2);
        CHECK_EQ(monthsFor, std::vector<int>{1, 2});
    }
    SUBCASE("Enter Data for one week")
    {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_day(t, "2024-07-08", "08:00", "16:00");
        insert_for_day(t, "2024-07-09", "08:00", "15:00");
        insert_for_day(t, "2024-07-10", "10:00", "16:30");
        insert_for_day(t, "2024-07-11", "09:00", "10:20");
        insert_for_day(t, "2024-07-11", "11:00", "13:00");
        insert_for_day(t, "2024-07-11", "18:00", "19:30");
        insert_for_day(t, "2024-07-12", "07:00", "13:00");
        auto result = t.overviewAsPointers("user_id_123", 2024, 7);
        CHECK_EQ(result.size(), 7);
        CHECK_EQ(result[0]->values()["date"], "08. July");
        CHECK_EQ(result[0]->values()["start_time"], "08:00");
        CHECK_EQ(result[0]->values()["end_time"], "16:00");
    }
    SUBCASE("Bug with Day Matching")
    {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17");
        insert_for_stop(t, "2024-07-13", "23:18");
        auto result = t.overviewAsPointers("user_id_123", 2024, 7);
        CHECK_EQ(result.size(), 2);
        CHECK_EQ(result[0]->values()["date"], "10. July");
        CHECK_EQ(result[0]->values()["start_time"], "23:55");
        CHECK_EQ(result[0]->values()["end_time"], "23:59");

    }
    SUBCASE("isOpen") {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17");
        insert_for_stop(t, "2024-07-13", "23:18");
        CHECK(t.isOpen("user_id_123", "2024-07-10").isOpen);
        CHECK_FALSE(t.isOpen("user_id_123", "2024-07-13").isOpen);
    }
    SUBCASE("Automatically close open days") {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17");
        insert_for_stop(t, "2024-07-13", "23:18");
        //t.closeOpenDays("user_id_123");
        auto result = t.overviewAsPointers("user_id_123", 2024, 7);
        CHECK_EQ(result.size(), 2);
        CHECK_EQ(result[0]->values()["date"], "10. July");
        CHECK_EQ(result[0]->values()["start_time"], "23:55");
        CHECK_EQ(result[0]->values()["end_time"], "23:59");
        CHECK_EQ(result[1]->values()["date"], "13. July");
        CHECK_EQ(result[1]->values()["start_time"], "23:17");
        CHECK_EQ(result[1]->values()["end_time"], "23:18");
    }
    SUBCASE("Automatically close open days except today")
    {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, String::currentDate(), "23:55");
        auto result = t.overviewAsPointers("user_id_123", 2024, 7);
        CHECK_EQ(result.size(), 1);
        CHECK_EQ(result[0]->values()["date"], String::convertDate(String::currentDate(), "%d. %B"));
        CHECK_EQ(result[0]->values()["start_time"], "23:55");
        CHECK_EQ(result[0]->values()["end_time"], "");
    }
}