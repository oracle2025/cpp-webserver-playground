#include "Data/Migrations.hpp"
#include "DateTime/Time.hpp"
#include "String/currentDateTime.hpp"
#include "TimeRecordingApp/TimeEntry.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
static const KeyStringType EMPLOYEE_ID = "employee_id";
static const KeyStringType EVENT_DATE = "event_date";
static const KeyStringType EVENT_TIME = "event_time";
static const KeyStringType EVENT_TYPE = "event_type";
static const KeyStringType NOTE = "note";
static const KeyStringType CREATION_DATE = "creation_date";
static const KeyStringType CREATOR_USER_ID = "creator_user_id";
static const KeyStringType CORRECTED_EVENT_ID = "corrected_event_id";
static const KeyStringType DELETED_EVENT_ID = "deleted_event_id";
static void insert_for_day(
    TimeEntry& t,
    const std::string& day,
    const std::string& start,
    const std::string& stop)
{
    t.set(EVENT_DATE, day);
    t.set(EVENT_TIME, start);
    t.set(EVENT_TYPE, "start");
    t.insert();
    t.set(EVENT_DATE, day);
    t.set(EVENT_TIME, stop);
    t.set(EVENT_TYPE, "stop");
    t.insert();
}
static void insert_for_start(
    TimeEntry& t, const std::string& day, const std::string& start)
{
    t.set(EVENT_DATE, day);
    t.set(EVENT_TIME, start);
    t.set(EVENT_TYPE, "start");
    t.insert();
}
static void insert_for_stop(
    TimeEntry& t, const std::string& day, const std::string& stop)
{
    t.set(EVENT_DATE, day);
    t.set(EVENT_TIME, stop);
    t.set(EVENT_TYPE, "stop");
    t.insert();
}
/*
 * What can go wrong:
 *
 * stop event before start event
 * start event before existing start-stop interval (or in the middle)
 * start stop event on different date than todays date?
 */

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
        t1.set("event_time", "12:00");
        t1.set("event_type", "start");
        t1.insert();
        TimeEntry t2;
        t2.set("employee_id", "user_id_123");
        t2.set("event_date", "2022-01-02");
        t2.set("event_time", "12:00");
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
        t1.set("event_time", "12:00");
        t1.set("event_type", "start");
        t1.insert();
        TimeEntry t2;
        t2.set("employee_id", "user_id_123");
        t2.set("event_date", "2021-02-02");
        t2.set("event_time", "12:00");
        t2.set("event_type", "start");
        t2.insert();
        const auto monthsFor = t1.monthsFor("user_id_123", 2021);
        CHECK_EQ(monthsFor.size(), 2);
        CHECK_EQ(monthsFor, std::vector<int>{1, 2});
    }
    SUBCASE("Enter Data for one week")
    {
        try {

            TimeEntry t;
            t.set("employee_id", "user_id_123");
            insert_for_day(t, "2024-07-08", "08:00", "16:00");
            insert_for_day(t, "2024-07-09", "08:00", "15:00");
            insert_for_day(t, "2024-07-10", "10:00", "16:30");
            insert_for_day(t, "2024-07-11", "09:00", "10:20");
            insert_for_day(t, "2024-07-11", "11:00", "13:00");
            insert_for_day(t, "2024-07-11", "18:00", "19:30");
            insert_for_day(t, "2024-07-12", "07:00", "13:00");
            auto result = t.overviewAsPointers(
                "user_id_123", 2024, 7, String::currentDate());
            CHECK_EQ(result.size(), 7);
            CHECK_EQ(result[0]->values()["date"], "8. Juli");
            CHECK_EQ(result[0]->values()["start_time"], "08:00");
            CHECK_EQ(result[0]->values()["end_time"], "16:00");
        } catch (...) {
            std::ostringstream str;
            Trace::backtrace(std::current_exception(), str);
            spdlog::error("Exception: {}", str.str());
            FAIL("Exception thrown");
        }
    }
    SUBCASE("Bug with Day Matching")
    {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17");
        insert_for_stop(t, "2024-07-13", "23:18");
        auto result = t.overviewAsPointers(
            "user_id_123", 2024, 7, String::currentDate());
        CHECK_EQ(result.size(), 2);
        CHECK_EQ(result[0]->values()["date"], "10. Juli");
        CHECK_EQ(result[0]->values()["start_time"], "23:55");
        CHECK_EQ(result[0]->values()["end_time"], "23:59");
    }
    SUBCASE("isOpen")
    {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17");
        insert_for_stop(t, "2024-07-13", "23:18");
        CHECK(t.isOpen("user_id_123", "2024-07-10").isOpen);
        CHECK_FALSE(t.isOpen("user_id_123", "2024-07-13").isOpen);
    }
    SUBCASE("Automatically close open days")
    {
        TimeEntry t;
        t.set("employee_id", "user_id_123");
        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17");
        insert_for_stop(t, "2024-07-13", "23:18");
        // t.closeOpenDays("user_id_123");
        auto result = t.overviewAsPointers(
            "user_id_123", 2024, 7, String::currentDate());
        CHECK_EQ(result.size(), 2);
        CHECK_EQ(result[0]->values()["date"], "10. Juli");
        CHECK_EQ(result[0]->values()["start_time"], "23:55");
        CHECK_EQ(result[0]->values()["end_time"], "23:59");
        CHECK_EQ(result[1]->values()["date"], "13. Juli");
        CHECK_EQ(result[1]->values()["start_time"], "23:17");
        CHECK_EQ(result[1]->values()["end_time"], "23:18");
    }
    SUBCASE("Automatically close open days except today")
    {
        for (auto& todays_date :
             {"2024-07-15",
              "2024-07-16",
              "2024-07-17",
              "2024-07-18",
              "2024-07-19",
              "2024-07-20",
              "2024-07-21"}) {
            Poco::Data::SQLite::Connector::registerConnector();
            Poco::Data::Session session("SQLite", ":memory:");
            g_session = &session;
            Data::MigrationsLatest m;
            m.perform();
            TimeEntry t;
            t.set("employee_id", "user_id_123");
            insert_for_start(t, todays_date, "23:55");
            auto result
                = t.overviewAsPointers("user_id_123", 2024, 7, todays_date);
            CHECK_EQ(result.size(), 1);
            CHECK_EQ(
                result[0]->values()["date"],
                String::convertDateToDayMonth(todays_date));
            CHECK_EQ(result[0]->values()["start_time"], "23:55");
            CHECK_EQ(result[0]->values()["end_time"], "");
        }
    }
    SUBCASE("Check negative difference between timestamps")
    {
        using std::tuple;
        using t = tuple<string, string, long>;
        using DateTime::Time;
        const auto test_data = {
            t{"9:00", "10:00", 60},
            t{"10:00", "9:00", -60},
            t{"9:00", "10:30", 90},
        };
        for (const auto& [start, end, expected] : test_data) {
            auto difference
                = Time::parseTime(end).difference(Time::parseTime(start));
            CHECK_EQ(difference.toMinutes(), expected);
        }
    }
    SUBCASE("Corrupted Time Calculation")
    {
        /*
BUG:

    Input:
        (employee_id = 'fceb73d4-630d-41bb-b253-bca924f07108' AND
strftime('%m-%Y', event_date) = '07-2024')

                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-10,23:55,start
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-13,23:17,start
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-13,23:18,stop
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-10,23:59,stop
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-14,21:31,start
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-14,21:35,stop
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-14,22:39,start
                                         fceb73d4-630d-41bb-b253-bca924f07108,2024-07-14,23:59,stop

*/
        TimeEntry t;
        t.set("employee_id", "user_id_123");

        insert_for_start(t, "2024-07-10", "23:55");
        insert_for_start(t, "2024-07-13", "23:17"); //,start
        insert_for_stop(t, "2024-07-13", "23:18"); //,stop
        insert_for_stop(t, "2024-07-10", "23:59"); //,stop
        insert_for_start(t, "2024-07-14", "21:31"); //,start
        insert_for_stop(t, "2024-07-14", "21:35"); //,stop
        insert_for_start(t, "2024-07-14", "22:39"); //,start
        insert_for_stop(t, "2024-07-14", "23:59"); //,stop

        auto records = t.overviewAsPointers(
            "user_id_123", 2024, 7, String::currentDate());

        std::ostringstream str;
        using DateTime::Time;
        for (const auto& entry : records) {
            const auto values = entry->values();
            const auto start_time = values.at("start_time");
            const auto end_time = values.at("end_time");
            str << values.at("day") << " " << values.at("date") << " "
                << values.at("start_time") << " " << values.at("end_time")
                << " " << ((start_time.empty() || end_time.empty())
                ? ""
                : Time::parseTime(end_time)
                      .difference(Time::parseTime(start_time))
                      .formatAsTotalHours()) << "\n";
        }
        //CHECK("" == str.str());

        /*
                                         Output:

            Mittwoch 	10. Juli 	23:55 - 23:18 	0 h -37 min
                Samstag 	13. Juli 	23:17 - 21:35 	-1 h -42 min
                Sonntag 	14. Juli 	21:31 - 23:59 	2 h 28 min
                Sonntag 	14. Juli 	22:39 -
            Summe 	1 h -51 min
         */
    }
}