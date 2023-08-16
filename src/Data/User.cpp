#include "User.hpp"

#include "RecordImpl.hpp"
#include "Trace/trace.hpp"
#include "backward.hpp"
#include "doctest.h"

#ifdef USE_POCO_CRYPTO
#include <Poco/Crypto/Cipher.h>
#include <Poco/PBKDF2Engine.h>
#include <Poco/HMACEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/DigestEngine.h>
#endif


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
#ifdef USE_POCO_CRYPTO
    Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> pbkdf2("salt", 4096, 256);
    pbkdf2.update("passphrase");
    Poco::DigestEngine::Digest d = pbkdf2.digest();
#endif

    this->password = password + salt;
}
string UserDefinition::table_name() const
{
    return "Users";
}

UserDefinition::UserDefinition(const UserDefinition::RecordType& d)
    : data{d}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
{
}
UserDefinition::UserDefinition(const UserDefinition& u)
    : data{u.data}
    , id(data.get<0>())
    , username(data.get<1>())
    , password(data.get<2>())
    , salt(data.get<3>())
{
}
void UserDefinition::set(const string& key, const string& value)
{
    if (key == "username") {
        username = value;
    } else if (key == "password") {
        setPassword(value);
    } else if (key == "salt") {
        salt = value;
    }
}
string UserDefinition::description() const
{
    return "User: " + username;
}
vector<string> UserDefinition::presentableFields() const
{
    return {"username"};
}
string UserDefinition::presentableName()
{
    return "User";
}
bool findUser(Poco::Data::Session& session, const string& username, User& user)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    using String::repeat;
    Statement select(session);
    string copy(username);
    select << "SELECT * FROM " + user.table_name() + " WHERE username = ?",
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
