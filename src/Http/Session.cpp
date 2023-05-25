
#include "Session.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"

namespace Http {

set<SessionId> Session::m_sessions;

Session::Session(const Request& request)
    : request(request)
{
}
bool Session::hasValidSession() const
{
    return request.hasCookie("session-id")
        && m_sessions.count(SessionId{request.cookie("session-id")});
}
void Session::clearSession()
{
    if (request.hasCookie("session-id")) {
        m_sessions.erase(SessionId{request.cookie("session-id")});
    }
}
void Session::createSession(Response& response)
{
    auto sessionId = generateRandomSessionId();
    m_sessions.insert(sessionId);
    response.cookie("session-id", sessionId);
}
} // namespace Http