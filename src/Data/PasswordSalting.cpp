#include "PasswordSalting.hpp"

#ifdef USE_POCO_CRYPTO
#include <Poco/Crypto/Cipher.h>
#include <Poco/PBKDF2Engine.h>
#include <Poco/HMACEngine.h>
#include <Poco/SHA1Engine.h>
#include <Poco/DigestEngine.h>
#endif

namespace Data {
PasswordSalting::PasswordSalting(const string& password, const string& salt)
{
#ifdef USE_POCO_CRYPTO
    Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> pbkdf2(salt, 4096, 256);
    pbkdf2.update(password);
    auto digest = pbkdf2.digest();
    m_hash = string(digest.begin(), digest.end());
#else
    string salted_password = password + salt;
    m_hash = Poco::Data::CLOB(salted_password);
#endif
}
Poco::Data::CLOB PasswordSalting::hash() const
{
    return m_hash;
}
bool PasswordSalting::isValid(const Poco::Data::CLOB& hash) const
{
    return hash == m_hash;
}
} // namespace Data