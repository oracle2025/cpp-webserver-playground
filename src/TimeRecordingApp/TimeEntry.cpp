
#include "TimeEntry.hpp"

#include "Data/Date.hpp"
#include "String/currentDateTime.hpp"

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

string convert_date_to_weekday(const string& input)
{
    return String::convertDate(input, "%A");
}

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
        = R"(SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
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
            convert_date_to_weekday(row.get<1>()),
            String::convertDate(row.get<1>(), "%d. %B"),
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
    select << sql, into(max_time), into(event_type_), bind(user_id), bind(date),
        now;
    return {event_type_ == "start", max_time};
}
void TimeEntryDefinition::closeOpenDays(const string& user_id)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    const auto sql = R"(SELECT
max_time, event_type, event_date
FROM
(
    SELECT
        MAX(event_time) AS max_time, event_type, event_date
    FROM
        time_events
    WHERE
        employee_id = ?
    GROUP BY
        event_date
) AS max_time_events
WHERE
event_type = 'start';)";
    using valueType = Poco::Tuple<string, string, string>;
    std::vector<valueType> result;
    Poco::Data::Statement select(*g_session);
    select << sql, into(result), bind(user_id), now;
    for (const auto& row : result) {
        if (row.get<1>() == "start" && row.get<2>() != String::currentDate()) {
            TimeEntry ted;
            ted.set("employee_id", user_id);
            ted.set("event_date", row.get<2>());
            ted.set("event_time", "23:59");
            ted.set("event_type", "stop");
            ted.insert();
        }
    }
}
