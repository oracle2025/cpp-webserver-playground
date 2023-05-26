
#include "SessionData.hpp"

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
void SessionData::login()
{
    m_isLoggedIn = true;
}
void SessionData::logout()
{
    m_isLoggedIn = false;
}
bool SessionData::isLoggedIn() const
{
    return m_isLoggedIn;
}
} // namespace Http