#pragma once

#include "RecordImpl.hpp"
#include "Data/PasswordSalting.hpp"

#include <Poco/Tuple.h>
#include <Poco/Data/LOB.h>

#include <string>

using std::string;

namespace Data {

class UserDefinition {
public:
    using RecordType = Poco::Tuple<string, string, Poco::Data::CLOB, string>;
    RecordType data;
    string& id;
    string& username;
    Poco::Data::CLOB& password;
    string& salt;
    //Poco::Data::BLOB password_hash;
    UserDefinition();
    vector<ColumnType> columns() const;
    string get(const KeyStringType& key) const;
    void setPassword(const string& password);
    string table_name() const;
    template<class T>
    static bool isValidUser(
        const string& username, const string& password, T& user)
    {
        if (findUser(*g_session, username, user)) {
            return PasswordSalting(password, user.salt).isValid(user.password);
        }
        return false;
    }
    void set(const KeyStringType& key, const string& value);
    string description() const;
    UserDefinition(const RecordType& d);
    UserDefinition(const UserDefinition& u);
    vector<KeyStringType> presentableFields() const;
};

using User = RecordImpl<UserDefinition>;

bool findUser(Poco::Data::Session& session, const string& username, User& user);

} // namespace Data