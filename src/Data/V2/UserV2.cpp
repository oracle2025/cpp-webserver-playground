#include "Data/RecordImpl.hpp"
#include "Trace/trace.hpp"
#include "UserV2.hpp"
#include "doctest.h"

namespace Data {
namespace V2 {

TEST_CASE("UserRecord")
{
    RecordImpl<UserDefinitionV2> user;
}
UserDefinitionV2::UserDefinitionV2()
    : data{"", "", {}, ""}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
{
}
vector<ColumnType> UserDefinitionV2::columns() const
{
    static const vector<ColumnType> cols =
     {
        {"username", "VARCHAR", HtmlInputType::TEXT},
        {"password", "BLOB", HtmlInputType::TEXT},
        {"salt", "VARCHAR", HtmlInputType::TEXT}};
    return cols;
}
string UserDefinitionV2::get(const KeyStringType& key) const
{
    if (key == "username") {
        return username;
    } else if (key == "password") {
        return "";
    } else if (key == "salt") {
        return salt;
    }
    return "";
}

void UserDefinitionV2::setPassword(const string& _password)
{
    salt = String::createRandomUUID();
    password = PasswordSalting(_password, salt).hash();
}
string UserDefinitionV2::table_name() const
{
    return "Users";
}
string UserDefinitionV2::presentableName() const
{
    return "Users";
}
UserDefinitionV2::UserDefinitionV2(const UserDefinitionV2::RecordType& d)
    : data{d}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
{
}
UserDefinitionV2::UserDefinitionV2(const UserDefinitionV2& u)
    : data{u.data}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
{
}
void UserDefinitionV2::set(const KeyStringType& key, const string& value)
{
    if (key == "username") {
        username = value;
    } else if (key == "password") {
        setPassword(value);
    } else if (key == "salt") {
        salt = value;
    }
}
string UserDefinitionV2::description() const
{
    return "User: " + username;
}
vector<KeyStringType> UserDefinitionV2::presentableFields() const
{
    return {"username"};
}
bool findUser(Poco::Data::Session& session, const string& username, UserV2& user)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    using String::repeat;
    Statement select(session);
    string copy(username);
    select << "SELECT id, username, password, salt FROM "
            + user.table_name() + " WHERE username = ?",
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
} // namespace V2
} // namespace Data