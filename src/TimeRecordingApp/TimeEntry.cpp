
#include "TimeEntry.hpp"

#include "Data/Date.hpp"
#include "Data/ValidationError.hpp"
#include "String/currentDateTime.hpp"
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <regex>
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

string TimeEntryDefinition::table_name() const
{
    return "time_events";
}
vector<ColumnType> TimeEntryDefinition::columns() const
{
    return {
        ColumnType{"employee_id", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"event_date", "DATE", HtmlInputType::DATE},
        ColumnType{"event_time", "TIME", HtmlInputType::TIME},
        ColumnType{"event_type", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"corrected_event_id", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"deleted_event_id", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"creation_date", "DATETIME", HtmlInputType::HIDDEN},
        ColumnType{"creator_user_id", "VARCHAR", HtmlInputType::HIDDEN},
    };
}
void TimeEntryDefinition::set(const KeyStringType& key, const string& value)
{
    if (key == "employee_id") {
        employee_id = value;
    } else if (key == "event_date") {
        event_date = value;
    } else if (key == "event_time") {
        event_time = value;
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
    }
}
string TimeEntryDefinition::get(const KeyStringType& key) const
{
    if (key == "employee_id") {
        return employee_id;
    } else if (key == "event_date") {
        return event_date;
    } else if (key == "event_time") {
        return event_time;
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
    }
    return {};
}
string TimeEntryDefinition::description() const
{
    return event_date + " " + event_time + " " + event_type + " " + employee_id;
}
vector<KeyStringType> TimeEntryDefinition::presentableFields()
{
    return {"event_date", "event_time", "event_type"};
}
string TimeEntryDefinition::presentableName()
{
    return "Time Entry";
}
bool TimeEntryDefinition::isEmptyFor(const string& user_id) const
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

std::vector<int> TimeEntryDefinition::yearsFor(const string& user_id) const
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

struct OverViewRow : public Record {
    OverViewRow(
        const string& day,
        const string& date,
        const string& start_time,
        const string& end_time)
        : m_day{day}
        , m_date{date}
        , m_start_time{start_time}
        , m_end_time{end_time}
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
        return {"day", "date", "start_time", "end_time"};
    };
    std::map<KeyStringType, string> values() const override
    {
        return {
            {"day", m_day},
            {"date", m_date},
            {"start_time", m_start_time},
            {"end_time", m_end_time}};
    };
    HtmlInputType inputType(const KeyStringType& field) const override
    {
        return HtmlInputType::TEXT;
    };
    string m_day, m_date, m_start_time, m_end_time;
};

vector<shared_ptr<Record>> TimeEntryDefinition::overviewAsPointers(
    const string& user_id, int year, int month)
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
    closeOpenDays(user_id);
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql
        = R"(SELECT tOn.employee_id, tOn.event_date,  tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (SELECT employee_id,
             event_time,
             event_date,
             ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
      FROM time_events
      where event_type = 'start' AND employee_id = ? AND strftime('%Y-%m', event_date) = ?) tOn
         LEFT JOIN (SELECT employee_id,
                           event_time,
                           event_date,
                           ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
                    FROM time_events
                    where event_type = 'stop' AND employee_id = ? AND strftime('%Y-%m', event_date) = ?) tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);)";
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
    std::vector<valueType> result;

    std::ostringstream year_month_str;
    year_month_str << year << "-" << std::setw(2) << std::setfill('0') << month;
    const auto year_month = year_month_str.str();

    Poco::Data::Statement select(*g_session);
    select << sql, into(result), bind(user_id), bind(year_month), bind(user_id),
        bind(year_month), now;
    std::vector<shared_ptr<Record>> records;
    records.reserve(result.size());
    for (const auto& row : result) {
        records.push_back(make_shared<OverViewRow>(
            String::convertDateToWeekday(row.get<1>()),
            String::convertDateToDayMonth(row.get<1>()),
            row.get<2>(),
            row.get<4>()));
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
MAX(event_time) AS max_time, event_type
FROM
time_events WHERE employee_id = ? AND event_date = ?
GROUP BY
event_date;)";
    string max_time;
    string event_type_;
    Statement select(*g_session);
    select << sql, into(max_time), into(event_type_), bind(user_id), bind(date);
    spdlog::debug("SQL EXEC: {}", select.execute());
    spdlog::debug("SQL: {}", select.toString());
    spdlog::debug("MAX_TIME: {}", max_time);
    spdlog::debug("event_type: {}", event_type_);
    return {event_type_ == "start", max_time};
}
void TimeEntryDefinition::closeOpenDays(const string& user_id)
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
        MAX(time(event_time)) AS max_time, event_type, event_date
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
    using valueType = Poco::Tuple<string, string, string>;
    std::vector<valueType> result;
    Poco::Data::Statement select(*g_session);
    select << sql, into(result), bind(user_id), now;
    for (const auto& row : result) {
        if (row.get<1>() == "start" && row.get<2>() != String::currentDate()) {
            spdlog::debug("Closing day: {}", row.get<2>());
            spdlog::debug("Closing time: {}", row.get<0>());
            spdlog::debug("currentDate day: {}", String::currentDate());
            TimeEntry ted;
            ted.set("employee_id", user_id);
            ted.set("event_date", row.get<2>());
            ted.set("event_time", "23:59");
            ted.set("event_type", "stop");
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

const auto regex_valid_time_str = "^([0-9]|0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$";

void TimeEntryDefinition::validate()
{
    std::regex r(regex_valid_time_str);
    if (!std::regex_match(event_time, r)) {
        throw Data::ValidationError(
            "Keine gültige Zeit: " + string(event_time));
    }
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
