
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
} // namespace Http