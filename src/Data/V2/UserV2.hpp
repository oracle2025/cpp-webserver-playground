#pragma once

#include "Data/PasswordSalting.hpp"
#include "Data/RecordImpl.hpp"

#include <Poco/Data/LOB.h>
#include <Poco/Tuple.h>

#include <string>

using std::string;

namespace Data {
namespace V2 {

class UserDefinitionV2 {
public:
    using RecordType
        = Poco::Tuple<string, string, Poco::Data::CLOB, string>;
    RecordType data;
    string& id;
    string& username;
    Poco::Data::CLOB& password;
    string& salt;
    // Poco::Data::BLOB password_hash;
    UserDefinitionV2();
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
    UserDefinitionV2(const RecordType& d);
    UserDefinitionV2(const UserDefinitionV2& u);
    vector<KeyStringType> presentableFields() const;
    void validate(){}
};

using UserV2 = RecordImpl<UserDefinitionV2>;

bool findUser(Poco::Data::Session& session, const string& username, UserV2& user);

} // namespace V2
} // namespace Data