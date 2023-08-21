#pragma once

#include "RecordImpl.hpp"
#include "Data/PasswordSalting.hpp"

#include <Poco/Tuple.h>

#include <string>

using std::string;

namespace Data {

class UserDefinition {
public:
    using RecordType = Poco::Tuple<string, string, string, string>;
    RecordType data;
    string& id;
    string& username;
    string& password;
    string& salt;
    UserDefinition();
    vector<ColumnType> columns() const;
    string get(const string& key) const;
    void setPassword(const string& password);
    string table_name() const;
    template<class T>
    bool isValidUser(
        const string& username, const string& password, T& user) const
    {
        if (findUser(*g_session, username, user)) {
            return PasswordSalting(password, user.salt).isValid(user.password);
        }
        return false;
    }
    void set(const string& key, const string& value);
    string description() const;
    UserDefinition(const RecordType& d);
    UserDefinition(const UserDefinition& u);
    vector<string> presentableFields() const;
    static string presentableName();
};

using User = RecordImpl<UserDefinition>;

bool findUser(Poco::Data::Session& session, const string& username, User& user);

} // namespace Data