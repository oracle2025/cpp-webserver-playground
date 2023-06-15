#include "User.hpp"

#include "RecordImpl.hpp"
#include "backward.hpp"
#include "doctest.h"

TEST_CASE("UserRecord")
{
    RecordImpl<UserDefinition> user;
}
UserDefinition::UserDefinition()
    : data{"", "", "", ""}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
{
}
vector<ColumnType> UserDefinition::columns() const
{
    return {
        {"username", "VARCHAR", HtmlInputType::TEXT},
        {"password", "VARCHAR", HtmlInputType::TEXT},
        {"salt", "VARCHAR", HtmlInputType::TEXT},
    };
}
string UserDefinition::get(const string& key) const
{
    if (key == "username") {
        return username;
    } else if (key == "password") {
        return password;
    } else if (key == "salt") {
        return salt;
    }
    return "";
}
void UserDefinition::setPassword(const string& password)
{
    salt = String::createRandomUUID();
    this->password = password + salt;
}
string UserDefinition::table_name() const
{
    return "Users";
}
bool UserDefinition::isValidUser(const string& user, const string& password)
{
    User u;
    if (findUser(*g_session, user, u)) {
        return u.password == password + u.salt;
    }
    return false;
}
bool findUser(Poco::Data::Session& session, const string& username, User& user)
{
    try {

        using namespace Poco::Data::Keywords;
        using Poco::Data::Statement;
        using String::repeat;
        Statement select(session);
        string copy(username);
        select << "SELECT * FROM " + user.table_name() + " WHERE username = ?",
            use(copy), into(user.data), range(0, 1);
        if (select.execute() == 0) {
            return false;
        }
        return true;
    } catch (Poco::Exception& e) {
        using namespace backward;
        StackTrace st;
        st.load_here(32);
        Printer p;
        p.print(st);
        std::cout << e.displayText() << std::endl;
        throw e;
    }
}
