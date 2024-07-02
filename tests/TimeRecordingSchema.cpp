#include "String/createRandomUUID.hpp"
#include "date/date.h"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
// Current Date time in the format 2024-06-13 08:45:00
std::string current_date_time()
{
    using namespace date;
    using namespace std::chrono;
    auto now = floor<seconds>(system_clock::now());
    std::ostringstream str;
    str << format("%F %T", now);
    return str.str();
}
void createTimeEventsTable(Poco::Data::Session& session)
{
    using namespace Poco::Data::Keywords;

    const auto createTimeEvents = R"(CREATE TABLE time_events (
id VARCHAR,
employee_id VARCHAR NOT NULL,
event_date DATE NOT NULL,
event_time TIME NOT NULL,
event_type VARCHAR CHECK( event_type IN ('start', 'stop', 'correction', 'deletion') ) NOT NULL DEFAULT 'start',
corrected_event_id VARCHAR,  -- References the event_id of the entry it corrects (if event_type = 'correction')
deleted_event_id VARCHAR,    -- References the event_id of the entry it deletes (if event_type = 'deletion')
creation_date DATETIME NOT NULL,
creator_user_id VARCHAR NOT NULL);
)";
    session << createTimeEvents, now;
}
std::string createTimeEvent(
    Poco::Data::Session& session,
    const std::string& employee_id,
    const std::string& event_date,
    const std::string& event_time,
    const std::string& event_type,
    const std::string& corrected_event_id,
    const std::string& deleted_event_id,
    const std::string& creator_user_id);
std::string createStartEvent(
    Poco::Data::Session& session,
    const std::string& employee_id,
    const std::string& event_date,
    const std::string& event_time,
    const std::string& creator_user_id)
{
    return createTimeEvent(
        session,
        employee_id,
        event_date,
        event_time,
        "start",
        "",
        "",
        creator_user_id);
}
std::string createStopEvent(
    Poco::Data::Session& session,
    const std::string& employee_id,
    const std::string& event_date,
    const std::string& event_time,
    const std::string& creator_user_id)
{
    return createTimeEvent(
        session,
        employee_id,
        event_date,
        event_time,
        "stop",
        "",
        "",
        creator_user_id);
}
std::string createCorrectionEvent(
    Poco::Data::Session& session,
    const std::string& employee_id,
    const std::string& event_date,
    const std::string& event_time,
    const std::string& corrected_event_id,
    const std::string& creator_user_id)
{
    return createTimeEvent(
        session,
        employee_id,
        event_date,
        event_time,
        "correction",
        corrected_event_id,
        "",
        creator_user_id);
}
std::string createDeletionEvent(
    Poco::Data::Session& session,
    const std::string& employee_id,
    const std::string& event_date,
    const std::string& event_time,
    const std::string& deleted_event_id,
    const std::string& creator_user_id)
{
    return createTimeEvent(
        session,
        employee_id,
        event_date,
        event_time,
        "deletion",
        "",
        deleted_event_id,
        creator_user_id);
}
std::string createTimeEvent(
    Poco::Data::Session& session,
    const std::string& employee_id,
    const std::string& event_date,
    const std::string& event_time,
    const std::string& event_type,
    const std::string& corrected_event_id,
    const std::string& deleted_event_id,
    const std::string& creator_user_id)
{
    using namespace Poco::Data::Keywords;
    auto id = String::createRandomUUID();
    const auto sql = R"(INSERT INTO time_events (
        id, employee_id, event_date, event_time, event_type,
        corrected_event_id, deleted_event_id, creation_date, creator_user_id) VALUES (
        ?, ?, ?, ?, ?, ?, ?, ?, ?); )";
    using string = std::string;
    using valueType = Poco::Tuple<
        string,
        string,
        string,
        string,
        string,
        string,
        string,
        string,
        string>;
    valueType data
        = {id,
           employee_id,
           event_date,
           event_time,
           event_type,
           corrected_event_id,
           deleted_event_id,
           current_date_time(),
           creator_user_id};
    Poco::Data::Statement insert(session);

    insert << sql, use(data), now;
    return id;
}

using string = std::string;
using valueType = Poco::Tuple<
    string,
    string,
    string,
    string,
    string,
    string,
    string,
    string,
    string>;
std::vector<valueType> selectIntoPairs(Poco::Data::Session& session)
{
    using namespace Poco::Data::Keywords;

    const auto sql
        = R"(SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (SELECT employee_id,
             event_time,
             event_date,
             ROW_NUMBER() Over (Partition by employee_id order by event_time) EventID
      FROM time_events
      where event_type = 'start') tOn
         LEFT JOIN (SELECT employee_id,
                           event_time,
                           event_date,
                           ROW_NUMBER() Over (Partition by employee_id order by event_time) EventID
                    FROM time_events
                    where event_type = 'stop') tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);)";

    std::vector<valueType> result;

    Poco::Data::Statement select(session);
    select << sql, into(result), now;
    return result;
}
/*
 * Corrections must have the same event_date as the event it corrects
 *
 * Corrections must have the same employee_id as the event it corrects
 *
 * Each event can have multiple corrections
 *
 * But each correction can only reference one event or another correction UNIQUE
 * constraint can be used because it supports <null>
 *
 * A deletion event must reference the last correction in the chain
 *
 * There must not be multiple start and stop events for the same employee_id and
 * event_date (maybe not)
 *
 * The event_time of the stop event must be greater than the start event
 *
 * If a start event does not have a stop event, the end of the day is assumed,
 * but a warning is issued
 *
 * Foreign key constraints are possible with:
 * PRAGMA foreign_keys = ON;
 * https://www.sqlite.org/foreignkeys.html
 */
TEST_CASE("Time Recording Schema")
{
    using namespace Poco::Data::Keywords;
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    createTimeEventsTable(session);

    SUBCASE("A Day with multiple Start and multiple Stop Events")
    {

        // https://stackoverflow.com/questions/69806206/sql-time-series-events-into-pairs
        createTimeEvent(
            session, "1", "2024-06-13", "08:15:00", "start", "", "", "2");
        createTimeEvent(
            session, "1", "2024-06-13", "12:10:00", "stop", "", "", "2");
        createTimeEvent(
            session, "1", "2024-06-13", "14:15:00", "start", "", "", "2");
        createTimeEvent(
            session, "1", "2024-06-13", "18:15:00", "stop", "", "", "2");

        const auto sql
            = R"(SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (
         SELECT employee_id, event_time, event_date,
                ROW_NUMBER() Over(Partition by employee_id order by event_time) EventID
         FROM time_events where event_type = 'start'
     ) tOn
         LEFT JOIN (
    SELECT employee_id, event_time, event_date,
           ROW_NUMBER() Over(Partition by employee_id order by event_time) EventID
    FROM time_events where event_type = 'stop'
) tOff
on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);)";

        std::vector<valueType> values;

        Poco::Data::Statement select(session);
        select << sql, into(values), now;

        CHECK(values.size() == 2);
        CHECK(values[0].get<0>() == "1");
        CHECK(values[0].get<1>() == "2024-06-13");
        CHECK(values[0].get<2>() == "08:15:00");
        CHECK(values[0].get<3>() == "2024-06-13");
        CHECK(values[0].get<4>() == "12:10:00");

        CHECK(values[1].get<0>() == "1");
        CHECK(values[1].get<1>() == "2024-06-13");
        CHECK(values[1].get<2>() == "14:15:00");
        CHECK(values[1].get<3>() == "2024-06-13");
        CHECK(values[1].get<4>() == "18:15:00");
    }
    SUBCASE("Select Time Events into Start Stop Pairs")
    {
    }
    SUBCASE("Select Time Events into Start Stop Pairs with Corrections applied")
    {
    }
    SUBCASE("Select Time Events into Start Stop Pairs with Corrections and "
            "Deletions applied")
    {
    }
    SUBCASE("Check for Orphane Stop Event")
    {
    }
    SUBCASE("Check for Stop Event before Start Event")
    {
        createStartEvent(session, "1", "2024-06-13", "08:15:00", "2");
        createStopEvent(session, "1", "2024-06-13", "08:10:00", "2");
    }

    SUBCASE("Select Time Events into Pairs with missing Stop Events")
    {
        createStartEvent(session, "1", "2024-06-13", "08:15:00", "2");
        createStopEvent(session, "1", "2024-06-13", "12:10:00", "2");
        createStartEvent(session, "1", "2024-06-14", "14:15:00", "2");
        auto values = selectIntoPairs(session);
        CHECK(values.size() == 2);
        CHECK(values[0].get<0>() == "1");
        CHECK(values[0].get<1>() == "2024-06-13");
        CHECK(values[0].get<2>() == "08:15:00");
        CHECK(values[0].get<3>() == "2024-06-13");
        CHECK(values[0].get<4>() == "12:10:00");

        CHECK(values[1].get<0>() == "1");
        CHECK(values[1].get<1>() == "2024-06-14");
        CHECK(values[1].get<2>() == "14:15:00");
        CHECK(values[1].get<3>() == ""); // Empty result can trigger a warning
        CHECK(values[1].get<4>() == "");
    }
    SUBCASE("Select Time Events with Corrections applied recursively")
    {

        // const auto testTimeEvents = R"(INSERT INTO time_events (employee_id,
        // event_date, event_time, event_type, creation_date,
        // creator_user_id))";
        const auto testTimeEvents
            = R"(INSERT INTO time_events (employee_id, event_date, event_time, event_type, corrected_event_id, deleted_event_id, creation_date, creator_user_id, id)
VALUES
    ('001', '2024-06-13', '08:00:00', 'start', NULL, NULL, '2024-06-13 08:30:00', '001', '001'),
    ('001', '2024-06-13', '17:00:00', 'stop', NULL, NULL, '2024-06-13 17:30:00', '002', '002'),
    ('002', '2024-06-13', '09:00:00', 'start', NULL, NULL, '2024-06-13 09:15:00', '001', '003'),
    ('002', '2024-06-13', '18:00:00', 'stop', NULL, NULL, '2024-06-13 18:30:00', '002', '004'),
    ('001', '2024-06-14', '08:30:00', 'start', NULL, NULL, '2024-06-14 08:45:00', '001', '005'),
    ('001', '2024-06-14', '17:30:00', 'stop', NULL, NULL, '2024-06-14 18:00:00', '002', '006'),
    ('001', '2024-06-13', '08:15:00', 'correction', '001', NULL, '2024-06-13 08:45:00', '002', '007'),
    ('001', '2024-06-14', '08:45:00', 'deletion', NULL, '005', '2024-06-14 09:00:00', '002', '008');
)";

        session << testTimeEvents, now;

        const auto selectCorrect = R"(WITH cteSteps AS (
SELECT c1.id as FinalID, c1.id, c1.corrected_event_id, c1.event_type as FinalEventType, c1.event_type, c1.event_date as FinalDate, c1.event_time as FinalTime, 1 as lvl
FROM time_events c1
UNION ALL
SELECT cte.FinalID, c2.id, c2.corrected_event_id, cte.FinalEventType, c2.event_type, c2.event_date,c2.event_time,cte.lvl + 1
FROM cteSteps cte
INNER JOIN time_events c2 ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
),
cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                FROM cteSteps
)
SELECT *
FROM cteNumbered
WHERE nr = 1 AND FinalEventType != 'correction' AND FinalEventType != 'deletion')";

        std::vector<valueType> values;

        Poco::Data::Statement select(session);
        select << selectCorrect, into(values), now;

        CHECK(values.size() == 6);
        CHECK(values[0].get<0>() == "1");
        CHECK(values[0].get<1>() == "001");
        CHECK(values[0].get<2>() == "007");
        CHECK(values[0].get<3>() == "001");
        CHECK(values[0].get<4>() == "start");
        CHECK(values[0].get<5>() == "correction");
        CHECK(values[0].get<6>() == "2024-06-13");
        CHECK(values[0].get<7>() == "08:15:00");
        CHECK(values[0].get<8>() == "2");
    }

    SUBCASE("Check for Multiple Corrections on the Same UUID")
    {
        auto id = createTimeEvent(
            session, "1", "2024-06-13", "08:00:00", "start", "", "", "1");

        createTimeEvent(
            session, "1", "2024-06-13", "08:15:00", "correction", id, "", "2");
        createTimeEvent(
            session, "1", "2024-06-13", "08:15:00", "correction", id, "", "2");

        const auto sql = R"(SELECT a.*
FROM time_events a
         JOIN (SELECT corrected_event_id, COUNT(*)
               FROM time_events
               GROUP BY corrected_event_id
               HAVING count(*) > 1 ) b
              ON a.corrected_event_id = b.corrected_event_id
ORDER BY a.corrected_event_id;)";

        std::vector<valueType> result;

        Poco::Data::Statement select(session);
        select << sql, into(result), now;
        CHECK(result.size() == 2);
    }

    SUBCASE("Check for Orphan Corrections")
    {
        auto id = createTimeEvent(
            session,
            "1",
            "2024-06-15",
            "09:00:00",
            "correction",
            "100",
            "",
            "1");
        const auto sql = R"(SELECT te.id AS correction_event_id,
te.corrected_event_id AS invalid_corrected_event_id
FROM time_events te
  LEFT JOIN time_events corrected_te ON te.corrected_event_id = corrected_te.id
WHERE te.event_type = 'correction'
AND corrected_te.id IS NULL)";

        std::vector<valueType> result;

        Poco::Data::Statement select(session);
        select << sql, into(result), now;
        CHECK(result.size() == 1);
    }
}
