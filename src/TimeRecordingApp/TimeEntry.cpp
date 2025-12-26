
#include "TimeEntry.hpp"

#include "Data/ValidationError.hpp"
#include "DateTime/Date.hpp"
#include "DateTime/Time.hpp"
#include "String/currentDateTime.hpp"

#include <optional>
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <regex>
#include <utility>
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
#include "doctest.h"
/*
 * TODO: Architecture Decision add id field to stop entries that matches start
 * entry start_event_id
 *
 * This way the link becomes explicit rather then implicit
 */

const auto regex_valid_time_str = "^([0-9]|0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$";

string TimeEntryDefinition::table_name()
{
    return "time_events";
}
vector<ColumnType> TimeEntryDefinition::columns()
{
    static const vector<ColumnType> cols = {
        ColumnType{"employee_id", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"event_date", "DATE", HtmlInputType::DATE},
        ColumnType{"event_time", "TIME", HtmlInputType::TIME},
        ColumnType{"event_type", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"corrected_event_id", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"deleted_event_id", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"creation_date", "DATETIME", HtmlInputType::HIDDEN},
        ColumnType{"creator_user_id", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"note", "VARCHAR", HtmlInputType::TEXT},
    };
    return cols;
}
void TimeEntryDefinition::set(const KeyStringType& key, const string& value)
{
    if (key == "employee_id") {
        employee_id = value;
    } else if (key == "event_date") {
        try {

            int timeZoneDifferential;
            auto dt = Poco::DateTimeParser::parse(
                "%Y-%m-%d", value, timeZoneDifferential);
            event_date = dt;
        } catch (...) {
            throw Data::ValidationError(
                "Keine gültiges Datum: " + string(value));
        }
    } else if (key == "event_time") {
        std::regex r(regex_valid_time_str);
        if (!std::regex_match(value, r)) {
            throw Data::ValidationError("Keine gültige Zeit: " + string(value));
        }
        int timeZoneDifferential;
        auto dt
            = Poco::DateTimeParser::parse("%H:%M", value, timeZoneDifferential);
        event_time = Poco::Data::Time{dt.hour(), dt.minute(), 0};
    } else if (key == "event_type") {
        event_type = value;
    } else if (key == "corrected_event_id") {
        corrected_event_id = value;
    } else if (key == "deleted_event_id") {
        deleted_event_id = value;
    } else if (key == "creation_date") {
        creation_date = value;
    } else if (key == "creator_user_id") {
        creator_user_id = value;
    } else if (key == "note") {
        note = value;
    }
}
string TimeEntryDefinition::get(const KeyStringType& key) const
{
    if (key == "employee_id") {
        return employee_id;
    } else if (key == "event_date") {
        return DateTime::Date(event_date).formatAsDate();
    } else if (key == "event_time") {
        return DateTime::Time(event_time).formatAsTime();
    } else if (key == "event_type") {
        return event_type;
    } else if (key == "corrected_event_id") {
        return corrected_event_id;
    } else if (key == "deleted_event_id") {
        return deleted_event_id;
    } else if (key == "creation_date") {
        return creation_date;
    } else if (key == "creator_user_id") {
        return creator_user_id;
    } else if (key == "note") {
        return note;
    }
    return {};
}
Poco::Data::Date TimeEntryDefinition::getEventDate() const
{
    return event_date;
}
string TimeEntryDefinition::description() const
{
    return get("event_date") + " " + get("event_time") + " " + event_type + " "
        + employee_id;
}
vector<KeyStringType> TimeEntryDefinition::presentableFields()
{
    return {"event_date", "event_time", "event_type"};
}
// Note: never called because of definition in RecordImpl
string TimeEntryDefinition::presentableName()
{
    return "Time Entry";
}
bool TimeEntryDefinition::isEmptyFor(const string& user_id)
try {
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const string sql = "SELECT COUNT(*) FROM time_events WHERE employee_id = ?";
    int count = 0;
    Statement select(*g_session);
    select << sql, into(count), bind(user_id);
    select.execute();
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("COUNT: {}", count);
    spdlog::debug("EMPLOYEE: {}", user_id);
    return count == 0;
} catch (...) {
    TRACE_RETHROW("Could not count");
}

std::vector<int> TimeEntryDefinition::yearsFor(const string& user_id)
try {
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql
        = "SELECT DISTINCT CAST(strftime('%Y', event_date) AS INTEGER) AS year "
          "FROM time_events WHERE year IS NOT null AND employee_id = ? "
          "ORDER BY year;";
    std::vector<int> result;
    Statement select(*g_session);
    select << sql, into(result), bind(user_id), now;
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("EMPLOYEE: {}", user_id);
    spdlog::debug("RESULT: {}", result.size());
    return result;
} catch (...) {
    TRACE_RETHROW("Could not list");
}

std::vector<int> TimeEntryDefinition::yearsForAllUsers() const
try {
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql
        = "SELECT DISTINCT CAST(strftime('%Y', event_date) AS INTEGER) AS year "
          "FROM time_events WHERE year IS NOT null "
          "ORDER BY year;";
    std::vector<int> result;
    Statement select(*g_session);
    select << sql, into(result), now;
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("RESULT: {}", result.size());
    return result;
} catch (...) {
    TRACE_RETHROW("Could not list");
}

std::vector<int> TimeEntryDefinition::monthsFor(
    const string& user_id, int year) const
try {
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql
        = "SELECT DISTINCT CAST(strftime('%m', event_date) AS INTEGER) AS "
          "month "
          "FROM time_events WHERE month IS NOT null AND employee_id = ? "
          "AND CAST(strftime('%Y', event_date) AS INTEGER) = ? "
          "ORDER BY month;";
    std::vector<int> result;
    Statement select(*g_session);
    select << sql, into(result), bind(user_id), bind(year), now;
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("EMPLOYEE: {}", user_id);
    spdlog::debug("YEAR: {}", year);
    spdlog::debug("RESULT: {}", result.size());
    return result;
} catch (...) {
    TRACE_RETHROW("Could not list");
}
[[nodiscard]] std::vector<int> TimeEntryDefinition::monthsForAllUsers(
    int year) const
try {
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql
        = "SELECT DISTINCT CAST(strftime('%m', event_date) AS INTEGER) AS "
          "month "
          "FROM time_events WHERE month IS NOT null "
          "AND CAST(strftime('%Y', event_date) AS INTEGER) = ? "
          "ORDER BY month;";
    std::vector<int> result;
    Statement select(*g_session);
    select << sql, into(result), bind(year), now;
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("YEAR: {}", year);
    spdlog::debug("RESULT: {}", result.size());
    return result;
} catch (...) {
    TRACE_RETHROW("Could not list");
}
struct OverViewRow : public Record {
    OverViewRow(
        string day,
        string date,
        const Poco::Data::Time& start_time,
        const std::optional<Poco::Data::Time>& end_time,
        const Poco::Data::Date& full_date,
        string start_id = {},
        string end_id = {},
        string note = {})
        : m_day{std::move(day)}
        , m_date{std::move(date)}
        , m_start_time{start_time}
        , m_end_time{end_time}
        , m_start_id{std::move(start_id)}
        , m_end_id{std::move(end_id)}
        , m_note{std::move(note)}
        , m_full_date{full_date}
    {
    }
    string presentableName() const override
    {
        return "OverViewRow";
    }
    string key() const override
    {
        return "";
    };
    std::vector<KeyStringType> fields() const override
    {
        static const std::vector<KeyStringType> fields = {"day", "date", "start_time", "end_time", "start_id", "end_id"};
        return fields;
    };
    std::map<KeyStringType, string> values() const override
    {
        using DateTime::Time;
        static const KeyStringType START_TIME_FIELD = "start_time";
        static const KeyStringType END_TIME_FIELD = "end_time";
        static const KeyStringType DATE_FIELD = "date";
        static const KeyStringType DAY_FIELD = "day";
        static const KeyStringType START_ID_FIELD = "start_id";
        static const KeyStringType END_ID_FIELD = "end_id";
        static const KeyStringType NOTE_FIELD = "note";
        return {
            {DAY_FIELD, m_day},
            {DATE_FIELD, m_date},
            {START_TIME_FIELD, Time(m_start_time).formatAsTime()},
            {END_TIME_FIELD,
             m_end_time.has_value() ? Time(m_end_time.value()).formatAsTime()
                                    : ""},
            {START_ID_FIELD, m_start_id},
            {END_ID_FIELD, m_end_id},
            {NOTE_FIELD, m_note}};
    };
    HtmlInputType inputType(const KeyStringType& field) const override
    {
        return HtmlInputType::TEXT;
    };
    string m_day, m_date;
    Poco::Data::Time m_start_time;
    std::optional<Poco::Data::Time> m_end_time;
    string m_start_id, m_end_id, m_note;
    Poco::Data::Date m_full_date;
};

vector<shared_ptr<Record>> TimeEntryDefinition::overviewAsPointers(
    const string& user_id, int year, int month, const string& current_date)
try {
    // Check the database for old and open time entries
    // And close them for 23:59
    /* Here is how to select the min/max time for each day:
     * SELECT
date_column,
MIN(time_column) AS min_time
FROM
your_table
GROUP BY
date_column;
     if the minimum is type start and the maximum is type stop, then the day is
closed
     */
    closeOpenDays(user_id, current_date);
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql = R"(
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (SELECT employee_id,
             event_time,
             event_date,
             ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
      FROM time_events
      where event_type = 'start'
        AND employee_id = ?
        AND strftime('%Y-%m', event_date) = ?) tOn
         LEFT JOIN (SELECT employee_id,
                           event_time,
                           event_date,
                           ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
                    FROM time_events
                    where event_type = 'stop'
                      AND employee_id = ?
                      AND strftime('%Y-%m', event_date) = ?) tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);
)";
    const auto sql_with_corrections_applied = R"(
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (WITH cteSteps AS (SELECT c1.id         as FinalID,
                               c1.id,
                               c1.employee_id,
                               c1.corrected_event_id,
                               c1.event_type as FinalEventType,
                               c1.event_type,
                               c1.event_date as FinalDate,
                               c1.event_time as FinalTime,
                               1             as lvl
                        FROM time_events c1
                        UNION ALL
                        SELECT cte.FinalID,
                               c2.id,
                               c2.employee_id,
                               c2.corrected_event_id,
                               cte.FinalEventType,
                               c2.event_type,
                               c2.event_date,
                               c2.event_time,
                               cte.lvl + 1
                        FROM cteSteps cte
                                 INNER JOIN time_events c2
                                            ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id),
           cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                           FROM cteSteps)
      SELECT employee_id,
             FinalTime      as event_time,
             FinalDate      as event_date,
             FinalEventType as event_type,
             ROW_NUMBER()      Over (Partition by employee_id order by FinalDate) EventID
      FROM cteNumbered
      WHERE nr = 1
        AND event_type = 'start'
        AND employee_id = ?
        AND strftime('%Y-%m', event_date) = ?) tOn
         LEFT JOIN (WITH cteSteps AS (SELECT c1.id         as FinalID,
                                             c1.id,
                                             c1.employee_id,
                                             c1.corrected_event_id,
                                             c1.event_type as FinalEventType,
                                             c1.event_type,
                                             c1.event_date as FinalDate,
                                             c1.event_time as FinalTime,
                                             1             as lvl
                                      FROM time_events c1
                                      UNION ALL
                                      SELECT cte.FinalID,
                                             c2.id,
                                             c2.employee_id,
                                             c2.corrected_event_id,
                                             cte.FinalEventType,
                                             c2.event_type,
                                             c2.event_date,
                                             c2.event_time,
                                             cte.lvl + 1
                                      FROM cteSteps cte
                                               INNER JOIN time_events c2
                                                          ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id),
                         cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                                         FROM cteSteps)
                    SELECT employee_id,
                           FinalTime      as event_time,
                           FinalDate      as event_date,
                           FinalEventType as event_type,
                           ROW_NUMBER()      Over (Partition by employee_id order by FinalDate) EventID
                    FROM cteNumbered
                    WHERE nr = 1
                      AND event_type = 'stop'
                      AND employee_id = ?
                      AND strftime('%Y-%m', event_date) = ?) tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);
)";
    /**
     * A Debug view, should have the results of the subqueries, the raw queries
     * with expanded binds to copy into sql console!
     *
     * And maybe the working "old" query as comparison
     *
     * Make testcases for all types if disordered timestamp rows
     *
     *
     */
    const auto sql_with_corrections_optimized_before_note_addition = R"(
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime, tOn.id StartId, tOff.id EndId
FROM (WITH cteSteps AS (SELECT c1.id         as FinalID,
                               c1.id,
                               c1.employee_id,
                               c1.corrected_event_id,
                               c1.event_type as FinalEventType,
                               c1.event_type,
                               c1.event_date as FinalDate,
                               c1.event_time as FinalTime,
                               1             as lvl
                        FROM time_events c1
                        WHERE employee_id = ?
                          AND strftime('%Y-%m', event_date) = ?
                        UNION ALL
                        SELECT cte.FinalID,
                               c2.id,
                               c2.employee_id,
                               c2.corrected_event_id,
                               cte.FinalEventType,
                               c2.event_type,
                               c2.event_date,
                               c2.event_time,
                               cte.lvl + 1
                        FROM cteSteps cte
                                 INNER JOIN time_events c2
                                            ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
                        WHERE c2.employee_id = ?
                          AND strftime('%Y-%m', event_date) = ?),
           cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                           FROM cteSteps)
      SELECT employee_id,
             FinalTime      as event_time,
             FinalDate      as event_date,
             FinalEventType as event_type,
             FinalID        as id,
             ROW_NUMBER()      Over (Partition by employee_id order by FinalDate, TIME(FinalTime)) EventID
      FROM cteNumbered
      WHERE nr = 1
        AND event_type = 'start') tOn
         LEFT JOIN (WITH cteSteps AS (SELECT c1.id         as FinalID,
                                             c1.id,
                                             c1.employee_id,
                                             c1.corrected_event_id,
                                             c1.event_type as FinalEventType,
                                             c1.event_type,
                                             c1.event_date as FinalDate,
                                             c1.event_time as FinalTime,
                                             1             as lvl
                                      FROM time_events c1
                                      WHERE employee_id = ?
                                        AND strftime('%Y-%m', event_date) = ?
                                      UNION ALL
                                      SELECT cte.FinalID,
                                             c2.id,
                                             c2.employee_id,
                                             c2.corrected_event_id,
                                             cte.FinalEventType,
                                             c2.event_type,
                                             c2.event_date,
                                             c2.event_time,
                                             cte.lvl + 1
                                      FROM cteSteps cte
                                               INNER JOIN time_events c2
                                                          ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
                                      WHERE c2.employee_id = ?
                                        AND strftime('%Y-%m', event_date) = ?),
                         cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                                         FROM cteSteps)
                    SELECT employee_id,
                           FinalTime      as event_time,
                           FinalDate      as event_date,
                           FinalEventType as event_type,
                           FinalID        as id,
                           ROW_NUMBER()      Over (Partition by employee_id order by FinalDate, TIME(FinalTime)) EventID
                    FROM cteNumbered
                    WHERE nr = 1
                      AND event_type = 'stop') tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);
)";
    const auto sql_with_corrections_optimized = R"(
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime, tOn.id StartId, tOff.id EndId, tOn.note StartNote, tOff.note EndNote
FROM (WITH cteSteps AS (SELECT c1.id         as FinalID,
                               c1.id,
                               c1.employee_id,
                               c1.note,
                               c1.corrected_event_id,
                               c1.event_type as FinalEventType,
                               c1.event_type,
                               c1.event_date as FinalDate,
                               c1.event_time as FinalTime,
                               1             as lvl
                        FROM time_events c1
                        WHERE employee_id = ?
                          AND strftime('%Y-%m', event_date) = ?
                        UNION ALL
                        SELECT cte.FinalID,
                               c2.id,
                               c2.employee_id,
                               c2.note,
                               c2.corrected_event_id,
                               cte.FinalEventType,
                               c2.event_type,
                               c2.event_date,
                               c2.event_time,
                               cte.lvl + 1
                        FROM cteSteps cte
                                 INNER JOIN time_events c2
                                            ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
                        WHERE c2.employee_id = ?
                          AND strftime('%Y-%m', event_date) = ?),
           cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                           FROM cteSteps)
      SELECT employee_id,
             note,
             FinalTime      as event_time,
             FinalDate      as event_date,
             FinalEventType as event_type,
             FinalID        as id,
             ROW_NUMBER()      Over (Partition by employee_id order by FinalDate, TIME(FinalTime)) EventID
      FROM cteNumbered
      WHERE nr = 1
        AND event_type = 'start') tOn
         LEFT JOIN (WITH cteSteps AS (SELECT c1.id         as FinalID,
                                             c1.id,
                                             c1.employee_id,
                                             c1.note,
                                             c1.corrected_event_id,
                                             c1.event_type as FinalEventType,
                                             c1.event_type,
                                             c1.event_date as FinalDate,
                                             c1.event_time as FinalTime,
                                             1             as lvl
                                      FROM time_events c1
                                      WHERE employee_id = ?
                                        AND strftime('%Y-%m', event_date) = ?
                                      UNION ALL
                                      SELECT cte.FinalID,
                                             c2.id,
                                             c2.employee_id,
                                             c2.note,
                                             c2.corrected_event_id,
                                             cte.FinalEventType,
                                             c2.event_type,
                                             c2.event_date,
                                             c2.event_time,
                                             cte.lvl + 1
                                      FROM cteSteps cte
                                               INNER JOIN time_events c2
                                                          ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
                                      WHERE c2.employee_id = ?
                                        AND strftime('%Y-%m', event_date) = ?),
                         cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                                         FROM cteSteps)
                    SELECT employee_id,
                           note,
                           FinalTime      as event_time,
                           FinalDate      as event_date,
                           FinalEventType as event_type,
                           FinalID        as id,
                           ROW_NUMBER()      Over (Partition by employee_id order by FinalDate, TIME(FinalTime)) EventID
                    FROM cteNumbered
                    WHERE nr = 1
                      AND event_type = 'stop') tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);
)";
    using valueType = Poco::Tuple<
        string, // employee_id 0
        Poco::Data::Date, // event_date 1
        Poco::Data::Time, // start_time 2
        Poco::Data::Date, // event_date 3
        string, // end_time 4
        string, // start_id 5
        string, // end_id 6
        string, // startnote 7
        string>; // endnote 8
    std::vector<valueType> result;

    std::ostringstream year_month_str;
    year_month_str << year << "-" << std::setw(2) << std::setfill('0') << month;
    const auto year_month = year_month_str.str();

    Poco::Data::Statement select(*g_session);
    select << sql_with_corrections_optimized, into(result), bind(user_id),
        bind(year_month), bind(user_id), bind(year_month), bind(user_id),
        bind(year_month), bind(user_id), bind(year_month), now;
    /*select << sql, into(result), bind(user_id),
        bind(year_month), bind(user_id), bind(year_month), now;*/
    std::vector<shared_ptr<Record>> records;
    records.reserve(result.size());
    for (const auto& row : result) {
        const auto eventDate = row.get<1>();
        const auto startTime = row.get<2>();
        const auto endTime = row.get<4>();
        const auto startId = row.get<5>();
        const auto endId = row.get<6>();
        const auto noteText = row.get<8>();
        const auto startNoteText = row.get<7>();

        Poco::Data::Time endTimeData;
        if (!endTime.empty()) {
            int timeZoneDifferential;
            auto dt = Poco::DateTimeParser::parse(
                "%H:%M:%S", endTime, timeZoneDifferential);
            endTimeData = Poco::Data::Time{dt.hour(), dt.minute(), 0};
        }
        using DateTime::Date;
        records.push_back(
            make_shared<OverViewRow>(
                Date(eventDate).formatAsWeekday(),
                Date(eventDate).formatAsDayMonth(),
                startTime,
                endTime.empty() ? std::nullopt
                                : std::make_optional(endTimeData),
                eventDate,
                startId,
                endId,
                endTime.empty() ? startNoteText : noteText));
    }
    return records;
} catch (...) {
    TRACE_RETHROW("Could not list");
}
TimeEntryDefinition::IsOpenResult TimeEntryDefinition::isOpen(
    const string& user_id, const string& date)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql = R"(SELECT
MAX(time(event_time)) AS max_time, event_type, note
FROM
time_events WHERE employee_id = ? AND event_date = ?
GROUP BY
event_date;)";
    Poco::Data::Time max_time;
    string event_type_;
    string note_;
    Statement select(*g_session);
    select << sql, into(max_time), into(event_type_), into(note_),
        bind(user_id), bind(date);
    spdlog::debug("SQL EXEC: {}", select.execute());
    const std::string event_time_str = DateTime::Time(max_time).formatAsTime();
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("MAX_TIME: {}", event_time_str);
    spdlog::debug("event_type: {}", event_type_);
    return {event_type_ == "start", event_time_str, note_};
}
void TimeEntryDefinition::closeOpenDays(
    const string& user_id, const string& current_date)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    /*
     * Does the following statement really do what I think it does?
     *
     * Does it really select the row where event_time is the maximum,
     * and shows the event_type that is associated with it.
     *
     * does MAX(event_time) really select the maximum time?
     *
     * In the below query, I would expect that the event_type matches with the
     * row that is selected by MAX(event_time)
     *
     * One problem in sorting is if a time
     * is entered as "8:00" instead of "08:00", note the leading zero
     */
    const auto sql = R"(SELECT
max_time, event_type, event_date
FROM
(
    SELECT
        MAX(time(event_time)) AS max_time, event_type, event_date, note
    FROM
        time_events
    WHERE
        employee_id = ?
    GROUP BY
        event_date
) AS max_time_events
WHERE
event_type = 'start';)";
    spdlog::debug("SQL: {}", sql);
    using valueType = Poco::Tuple<string, string, string, string>;
    std::vector<valueType> result;
    Poco::Data::Statement select(*g_session);
    select << sql, into(result), bind(user_id), now;
    static const KeyStringType EMPLOYEE_ID = "employee_id";
    static const KeyStringType EVENT_DATE = "event_date";
    static const KeyStringType EVENT_TIME = "event_time";
    static const KeyStringType EVENT_TYPE = "event_type";
    static const KeyStringType NOTE = "note";
    for (const auto& row : result) {
        const auto event_type_ = row.get<1>();
        const auto event_date_ = row.get<2>();
        const auto note_ = row.get<3>();
        if (event_type_ == "start" && event_date_ != current_date) {
            spdlog::debug("Closing day: {}", row.get<2>());
            spdlog::debug("Closing time: {}", row.get<0>());
            spdlog::debug("currentDate day: {}", current_date);
            TimeEntry ted;
            ted.set(EMPLOYEE_ID, user_id);
            ted.set(EVENT_DATE, event_date_);
            ted.set(NOTE, note_);
            ted.set(EVENT_TIME, "23:59");
            ted.set(EVENT_TYPE, "stop");
            ted.insert();
        }
    }
}
bool TimeEntryDefinition::checkTimestampExists(
    const string& employee_id_,
    const string& event_date_,
    const string& event_time_)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql
        = R"(SELECT COUNT(*) FROM time_events WHERE employee_id = ? AND event_date = ? AND event_time = ?;)";
    int count = 0;
    Statement select(*g_session);
    select << sql, into(count), bind(employee_id_), bind(event_date_),
        bind(event_time_);
    select.execute();
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("COUNT: {}", count);
    spdlog::debug("EMPLOYEE: {}", employee_id_);
    spdlog::debug("DATE: {}", event_date_);
    spdlog::debug("TIME: {}", event_time_);
    if (count > 0) {
        return true;
    }
    return false;
}

void TimeEntryDefinition::validate()
{
}

vector<shared_ptr<Record>> TimeEntryDefinition::listDay(
    const string& user_id, const string& current_date)
try {
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql = R"(
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (SELECT employee_id,
     event_time,
     event_date,
     ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
FROM time_events
where event_type = 'start'
AND employee_id = ?
AND strftime('%Y-%m-%d', event_date) = ?) tOn
 LEFT JOIN (SELECT employee_id,
                   event_time,
                   event_date,
                   ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
            FROM time_events
            where event_type = 'stop'
              AND employee_id = ?
              AND strftime('%Y-%m-%d', event_date) = ?) tOff
           on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);
)";
    using valueType = Poco::Tuple<
        string, // employee_id 0
        Poco::Data::Date, // event_date 1
        Poco::Data::Time, // start_time 2
        Poco::Data::Date, // event_date 3
        string, // end_time 4
        string,
        string,
        string,
        string>;
    std::vector<valueType> result;

    std::ostringstream year_month_day;

    Poco::Data::Statement select(*g_session);
    select << sql, into(result), bind(user_id), bind(current_date),
        bind(user_id), bind(current_date), now;
    std::vector<shared_ptr<Record>> records;
    records.reserve(result.size());
    for (const auto& row : result) {
        const auto eventDate = row.get<1>();
        const auto startTime = row.get<2>();
        const auto endTime = row.get<4>();
        const auto startId = row.get<5>();
        const auto endId = row.get<6>();
        const auto noteText = row.get<8>();
        const auto startNoteText = row.get<7>();

        Poco::Data::Time endTimeData;
        if (!endTime.empty()) {
            int timeZoneDifferential;
            auto dt = Poco::DateTimeParser::parse(
                "%H:%M:%S", endTime, timeZoneDifferential);
            endTimeData = Poco::Data::Time{dt.hour(), dt.minute(), 0};
        }
        using DateTime::Date;
        records.push_back(
            make_shared<OverViewRow>(
                Date(eventDate).formatAsWeekday(),
                Date(eventDate).formatAsDayMonth(),
                startTime,
                endTime.empty() ? std::nullopt
                                : std::make_optional(endTimeData),
                eventDate,
                startId,
                endId,
                endTime.empty() ? startNoteText : noteText));
    }
    return records;
} catch (...) {
    TRACE_RETHROW("Could not list");
}
bool TimeEntryDefinition::checkTimerangeOverlaps(
    const string& employee_id_,
    const string& event_date_,
    const string& start_time,
    const string& end_time)
{
    /* Types of Overlap:
     *
     * 1) No Overlap end
     * A: -----+======+------------
     * B: --------------+======+---
     *
     * B.start_time > A.end_time
     *
     * 2) No Overlap start
     * A: --------------+======+---
     * B: -----+======+------------
     *
     * B.end_time < A.start_time
     *
     * 3) Complete Overlap
     * A: -----+======+------------
     * B: -------+===+-------------
     *
     * B.start_time >= A.start_time && B.end_time <= A.end_time
     *
     * 4) Partial Overlap Start and End
     * A: -----+======+------------
     * B: ---+==========+----------
     *
     * B.start_time < A.start_time && B.end_time > A.end_time
     *
     * 5) Partial Overlap Start
     * A: -----+======+------------
     * B: ---+====+----------------
     *
     * B.start_time < A.start_time && B.end_time >= A.start_time
     *
     * 6) Partial Overlap End
     * A: -----+======+------------
     * B: ----------+====+---------
     *
     * B.start_time <= A.end_time && B.end_time > A.end_time
     *
     * if B.end_time >= A.start_time && B.start_time <= A.end_time
     */
    // ASSERT: start_time < end_time
    // I would need to get all the pairs for a day
    auto all_pairs_for_day = listDay(employee_id_, event_date_);

    struct time_span {
        DateTime::Time start_time;
        DateTime::Time end_time;
    };

    time_span A{
        DateTime::Time::parseTime(start_time),
        DateTime::Time::parseTime(end_time)};

    for (const auto& pair : all_pairs_for_day) {
        auto startTime = pair->values().at("start_time");
        auto endTime = pair->values().at("end_time");
        time_span B{
            DateTime::Time::parseTime(startTime),
            DateTime::Time::parseTime(endTime)};
        spdlog::debug(
            "CheckOverlap: {} {} {}", event_date_, startTime, endTime);
        // Complete Overlap
        if (B.start_time >= A.start_time && B.end_time <= A.end_time) {
                return true;
        }
        // Partial Overlap Start and End
        if (B.start_time < A.start_time && B.end_time > A.end_time) {
            return true;
        }
        // Partial Overlap Start
        if (B.start_time < A.start_time && B.end_time >= A.start_time) {
            return true;
        }
        // Partial Overlap End
        if (B.start_time <= A.end_time && B.end_time > A.end_time) {
            return true;
        }
    }
    return false;
}

TEST_CASE("Valid Time Regex")
{
    std::regex r(regex_valid_time_str);
    CHECK(std::regex_match("08:00", r));
    CHECK(std::regex_match("23:59", r));
    CHECK(std::regex_match("00:00", r));
    CHECK(std::regex_match("12:00", r));
    CHECK(std::regex_match("01:00", r));
    CHECK(std::regex_match("09:00", r));
    CHECK(std::regex_match("19:00", r));
    CHECK(std::regex_match("20:00", r));
    CHECK(std::regex_match("21:00", r));
    CHECK(std::regex_match("22:00", r));
    CHECK(std::regex_match("23:00", r));
    CHECK(std::regex_match("00:59", r));
    CHECK(std::regex_match("00:01", r));
}
