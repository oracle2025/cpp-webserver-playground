#include "PasswordSalting.hpp"

#include "String/createRandomUUID.hpp"
#include "doctest.h"
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
TEST_CASE("PasswordSalting")
{
    auto salt = String::createRandomUUID();
    auto passphrase = "passphrase";
    auto hash = PasswordSalting(passphrase, salt).hash();
    CHECK(PasswordSalting(passphrase, salt).isValid(hash));
}
#ifdef USE_POCO_CRYPTO
TEST_CASE("Crypto")
{
    auto salt = String::createRandomUUID();
    auto passphrase = "passphrase";
    Poco::PBKDF2Engine<Poco::HMACEngine<Poco::SHA1Engine>> pbkdf2(
        salt, 4096, 256);
    pbkdf2.update(passphrase);
    Poco::DigestEngine::Digest d = pbkdf2.digest();
    auto actual = string{d.begin(), d.end()};
    pbkdf2.update(passphrase);
    d = pbkdf2.digest();
    auto expected = string{d.begin(), d.end()};
    CHECK(actual == expected);
}
#endif
} // namespace Data
