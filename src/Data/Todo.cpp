#include "Todo.hpp"

#include <Poco/Data/Session.h>

#include <chrono>
#include <iostream>
#include <vector>

using Poco::Data::Session;
using Poco::Data::Statement;

using namespace std;

string time_string()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    return std::ctime(&t_c);
}

string TodoDefinition::description() const
{
    return m_description;
}
string TodoDefinition::get(const KeyStringType& key) const
{
    if (key == "id") {
        return id;
    } else if (key == "description") {
        return m_description;
    } else if (key == "created_at") {
        return created_at;
    } else if (key == "updated_at") {
        return updated_at;
    } else if (key == "checked") {
        return checked ? "yes" : "no";
    } else if (key == "user_id") {
        return user_id;
    }
    return "";
}
void TodoDefinition::set(const KeyStringType& key, const string& value)
{
    if (key == "id") {
        id = value;
    } else if (key == "description") {
        m_description = value;
    } else if (key == "created_at") {
        created_at = value;
    } else if (key == "updated_at") {
        updated_at = value;
    } else if (key == "checked") {
        value == "yes" ? checked = true : checked = false;
    } else if (key == "user_id") {
        user_id = value;
    }
}
vector<ColumnType> TodoDefinition::columns() const
{
    return {
        ColumnType{"description", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"created_at", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"updated_at", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"checked", "INTEGER(3)", HtmlInputType::CHECKBOX},
        ColumnType{"user_id", "VARCHAR", HtmlInputType::HIDDEN},
    };
}
string TodoDefinition::table_name() const
{
    return "Todo";
}
vector<KeyStringType> TodoDefinition::presentableFields()
{
    return vector<KeyStringType>{"checked", "description"};
}
string TodoDefinition::presentableName()
{
    return "Todo";
}
