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
    m_hash = pbkdf2.digest();
#else
    m_hash = password + salt;
#endif
}
string PasswordSalting::hash() const
{
    return m_hash;
}
bool PasswordSalting::isValid(const string& hash) const
{
    return hash == m_hash;
}
} // namespace Data