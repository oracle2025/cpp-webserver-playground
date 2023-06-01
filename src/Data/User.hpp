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
    bool isValidUser(const string& user, const string& password);
};

using User = RecordImpl<UserDefinition>;

bool findUser(Poco::Data::Session& session, const string& username, User& user);
