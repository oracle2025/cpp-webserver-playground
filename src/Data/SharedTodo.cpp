#include "SharedTodo.hpp"

#include <Poco/Data/Session.h>

#include <chrono>
#include <iostream>
#include <vector>

using Poco::Data::Session;
using Poco::Data::Statement;

using namespace std;


string SharedTodoDefinition::description() const
{
    return m_description;
}
string SharedTodoDefinition::get(const KeyStringType& key) const
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
void SharedTodoDefinition::set(const KeyStringType& key, const string& value)
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
vector<ColumnType> SharedTodoDefinition::columns() const
{
    return {
        ColumnType{"description", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"created_at", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"updated_at", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"checked", "INTEGER(3)", HtmlInputType::CHECKBOX},
        ColumnType{"user_id", "VARCHAR", HtmlInputType::HIDDEN},
    };
}
string SharedTodoDefinition::table_name() const
{
    return "SharedTodo";
}
vector<KeyStringType> SharedTodoDefinition::presentableFields()
{
    return vector<KeyStringType>{"checked", "description"};
}
string SharedTodoDefinition::presentableName()
{
    return "Todo";
}
