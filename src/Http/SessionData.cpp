
#include "SessionData.hpp"
#include <chrono>

namespace Http {
void SessionData::alert(const SessionData::Alert& alert)
{
    m_alert = alert;
}
void SessionData::clearAlert()
{
    m_alert = {};
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
}
void SessionData::logout()
{
    m_isLoggedIn = false;
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
} // namespace Http