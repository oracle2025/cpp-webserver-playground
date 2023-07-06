#pragma once

#include "RecordImpl.hpp"

#include <Poco/Tuple.h>

#include <string>

using std::string;

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
    bool isValidUser(const string& username, const string& password, T& user) const
    {
        if (findUser(*g_session, username, user)) {
            return user.password == password + user.salt;
        }
        return false;
    }
    void set(const string& key, const string& value);
    string description() const;
    UserDefinition(const RecordType& d);
    UserDefinition(const UserDefinition& u);
};

using User = RecordImpl<UserDefinition>;

bool findUser(Poco::Data::Session& session, const string& username, User& user);
