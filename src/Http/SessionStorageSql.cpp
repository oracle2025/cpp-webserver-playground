#include "SessionStorageSql.hpp"

#include "Data/SessionRow.hpp"

namespace Http {

using Data::SessionRow;

bool SessionStorageSql::pop(const SessionId& sessionId)
{
    SessionRow r;
    if (r.pop(sessionId)) {
        m_current = r.toSessionData();
        return true;
    }
    return false;
}
size_t SessionStorageSql::count(const SessionId& sessionId)
{
    SessionRow r;
    return r.pop(sessionId);
}
SessionData& SessionStorageSql::data()
{
    return m_current;
}
void SessionStorageSql::erase(const SessionId& sessionId)
{
    SessionRow r;
    if (r.pop(sessionId)) {
        r.erase();
    }
}
void SessionStorageSql::insert(
    const SessionId& sessionId, const SessionData& sessionData)
{
    SessionRow r;
    if (r.pop(sessionId)) {
        r.fromSessionData(sessionData);
        r.update();
    } else {
        r.fromSessionData(sessionData);
        r.insert(sessionId);
    }
}
void SessionStorageSql::clearAll()
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    Statement erase(*g_session);
    erase << "DELETE FROM Session", now;
}
vector<shared_ptr<Record>> SessionStorageSql::listAll()
{
    SessionRow r;
    return r.listAsPointers();
}

} // namespace Http