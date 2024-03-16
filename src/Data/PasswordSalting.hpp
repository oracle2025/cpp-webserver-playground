#pragma once

#include <iosfwd>
#include <Poco/Data/LOB.h>

namespace Data {

using std::string;

class PasswordSalting {
public:
    PasswordSalting(const string& password, const string& salt);
    Poco::Data::CLOB hash() const;
    bool isValid(const Poco::Data::CLOB& hash) const;
private:
    Poco::Data::CLOB m_hash;
};

} // namespace Data

