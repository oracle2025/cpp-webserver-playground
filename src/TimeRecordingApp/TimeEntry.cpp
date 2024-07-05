
#include "TimeEntry.hpp"

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