#pragma once

#include <string>

namespace Data {

using std::string;

class PasswordSalting {
public:
    PasswordSalting(const string& password, const string& salt);
    string hash() const;
    bool isValid(const string& hash) const;
private:
    string m_hash;
};

} // namespace Data

