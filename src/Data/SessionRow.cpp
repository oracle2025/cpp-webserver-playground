

#include "SessionRow.hpp"

namespace Data {

string SessionRowDefinition::table_name() const
{
    return "Session";
}
vector<ColumnType> SessionRowDefinition::columns() const
{
    return {
        ColumnType{"isLoggedIn", "INTEGER(3)", HtmlInputType::CHECKBOX},
        ColumnType{"userId", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"userName", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"createdAt", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"lastUsedAt", "VARCHAR", HtmlInputType::HIDDEN},
        ColumnType{"path", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"userAgent", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"hasAlert", "INTEGER(3)", HtmlInputType::CHECKBOX},
        ColumnType{"alert", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"alertType", "VARCHAR", HtmlInputType::TEXT},
    };
}
void SessionRowDefinition::set(const KeyStringType& key, const string& value)
{
    if (key == "id") {
        id = value;
    } else if (key == "isLoggedIn") {
        isLoggedIn = value == "yes";
    } else if (key == "userId") {
        userId = value;
    } else if (key == "userName") {
        userName = value;
    } else if (key == "createdAt") {
        createdAt = value;
    } else if (key == "lastUsedAt") {
        lastUsedAt = value;
    } else if (key == "path") {
        path = value;
    } else if (key == "userAgent") {
        userAgent = value;
    } else if (key == "hasAlert") {
        hasAlert = value == "yes";
    } else if (key == "alert") {
        alert = value;
    } else if (key == "alertType") {
        alertType = value;
    }
}
string SessionRowDefinition::get(const KeyStringType& key) const
{
    if (key == "id") {
        return id;
    } else if (key == "isLoggedIn") {
        return isLoggedIn ? "yes" : "no";
    } else if (key == "userId") {
        return userId;
    } else if (key == "userName") {
        return userName;
    } else if (key == "createdAt") {
        return createdAt;
    } else if (key == "lastUsedAt") {
        return lastUsedAt;
    } else if (key == "path") {
        return path;
    } else if (key == "userAgent") {
        return userAgent;
    } else if (key == "hasAlert") {
        return hasAlert ? "yes" : "no";
    } else if (key == "alert") {
        return alert;
    } else if (key == "alertType") {
        return alertType;
    }
    return "";
}
string SessionRowDefinition::description() const
{
    return id + ": " + userName + " " + createdAt + " " + lastUsedAt;
}
vector<KeyStringType> SessionRowDefinition::presentableFields()
{
    return {
        "id",
        "isLoggedIn",
        "userId",
        "userName",
        "createdAt",
        "lastUsedAt",
        "path",
        "userAgent",
        "hasAlert",
        "alert",
        "alertType",
    };
}
Http::SessionData SessionRowDefinition::toSessionData() const
{
    Http::SessionData result{SessionId{id}, path, userAgent};
    result.m_isLoggedIn = isLoggedIn;
    result.m_userId = userId;
    result.m_userName = userName;
    result.m_createdAt = std::stol(createdAt);
    result.m_lastUsedAt = std::stol(lastUsedAt);
    if (hasAlert) {
        result.m_alert = Html::Alert{alert, Html::Alert::fromString(alertType)};
    } else {
        result.m_alert.reset();
    }
    return result;
}
void SessionRowDefinition::fromSessionData(const Http::SessionData& sessionData)
{
    id = sessionData.m_id;
    isLoggedIn = sessionData.m_isLoggedIn;
    userId = sessionData.m_userId;
    userName = sessionData.m_userName;
    createdAt = std::to_string(sessionData.m_createdAt);
    lastUsedAt = std::to_string(sessionData.m_lastUsedAt);
    path = sessionData.m_path;
    userAgent = sessionData.m_userAgent;
    hasAlert = sessionData.m_alert.has_value();
    if (hasAlert) {
        alert = sessionData.m_alert.value().message();
        alertType = sessionData.m_alert.value().typeAsString();
    }
}
string SessionRowDefinition::presentableName()
{
    return "Session";
}

} // namespace Data