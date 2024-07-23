#include "User.hpp"

#include "RecordImpl.hpp"
#include "Trace/trace.hpp"
#include "doctest.h"

namespace Data {

TEST_CASE("UserRecord")
{
    RecordImpl<UserDefinition> user;
}
UserDefinition::UserDefinition()
    : data{"", "", {}, ""}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
    , start_page(data.get<4>())
{
}
vector<ColumnType> UserDefinition::columns() const
{
    return {
        {"username", "VARCHAR", HtmlInputType::TEXT},
        {"password", "BLOB", HtmlInputType::HIDDEN},
        {"salt", "VARCHAR", HtmlInputType::HIDDEN},
        {"start_page", "VARCHAR", HtmlInputType::TEXT}};
}
string UserDefinition::get(const KeyStringType& key) const
{
    if (key == "username") {
        return username;
    } else if (key == "password") {
        return "";
    } else if (key == "salt") {
        return salt;
    } else if (key == "start_page") {
        return start_page;
    }
    return "";
}

void UserDefinition::setPassword(const string& _password)
{
    salt = String::createRandomUUID();
    password = PasswordSalting(_password, salt).hash();
}
string UserDefinition::table_name() const
{
    return "Users";
}
string UserDefinition::presentableName() const
{
    return "User";
}
UserDefinition::UserDefinition(const UserDefinition::RecordType& d)
    : data{d}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
    , start_page(data.get<4>())
{
}
UserDefinition::UserDefinition(const UserDefinition& u)
    : data{u.data}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
    , start_page(data.get<4>())
{
}
void UserDefinition::set(const KeyStringType& key, const string& value)
{
    if (key == "username") {
        username = value;
    } else if (key == "password") {
        setPassword(value);
    } else if (key == "salt") {
        salt = value;
    } else if (key == "start_page") {
        start_page = value;
    }
}
string UserDefinition::description() const
{
    return "User: " + username;
}
vector<KeyStringType> UserDefinition::presentableFields() const
{
    return {"username"};
}
bool findUser(Poco::Data::Session& session, const string& username, User& user)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    using String::repeat;
    Statement select(session);
    string copy(username);
    select << "SELECT id, username, password, salt, start_page FROM " + user.table_name()
            + " WHERE username = ?",
        use(copy), into(user.data), range(0, 1);
    try {
        if (select.execute() == 0) {
            return false;
        }
        return true;
    } catch (...) {
        TRACE_RETHROW("Could not find user");
    }
}
} // namespace Data