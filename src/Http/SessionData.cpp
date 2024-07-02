
#include "SessionData.hpp"

#include "SessionStorage.hpp"

#include <chrono>

namespace Http {
void SessionData::alert(const SessionData::Alert& alert)
{
    m_alert = alert;
    SessionStorage::insert(m_id, *this);
}
void SessionData::clearAlert()
{
    m_alert = {};
    SessionStorage::insert(m_id, *this);
}
bool SessionData::hasAlert() const
{
    return m_alert.has_value();
}
const SessionData::Alert& SessionData::getAlert() const
{
    return m_alert.value();
}
void SessionData::login(Data::User& user)
{
    m_isLoggedIn = true;
    m_userId = user.id;
    m_userName = user.username;
    SessionStorage::insert(m_id, *this);
}
void SessionData::logout()
{
    m_isLoggedIn = false;
    SessionStorage::insert(m_id, *this);
}
bool SessionData::isLoggedIn()
{
    touch();
    return m_isLoggedIn;
}
string SessionData::userId() const
{
    return m_userId;
}
string SessionData::userName() const
{
    return m_userName;
}
std::time_t SessionData::now()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}
void SessionData::touch()
{
    m_lastUsedAt = now();
    SessionStorage::insert(m_id, *this);
}
string SessionData::createdAt() const
{
    return std::ctime(&m_createdAt);
}
string SessionData::lastUsedAt() const
{
    return std::ctime(&m_lastUsedAt);
}
bool SessionData::isLoggedInConst() const
{
    return m_isLoggedIn;
}
SessionData::SessionData(
    SessionId  id, const string path, const string& userAgent)
    : m_path(path)
    , m_userAgent(userAgent)
    , m_id(std::move(id))
{
}
string SessionData::path() const
{
    return m_path;
}
string SessionData::userAgent() const
{
    return m_userAgent;
}
} // namespace Http