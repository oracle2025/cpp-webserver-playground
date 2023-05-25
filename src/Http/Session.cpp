
#include "Session.hpp"

#include "Http/Request.hpp"
#include "Http/Response.hpp"

namespace Http {

map<SessionId, SessionData> Session::m_sessions;

Session::Session(const Request& request)
    : request(request)
{
}
bool Session::hasValidSession() const
{
    return request.hasCookie("session-id")
        && m_sessions.find(SessionId{request.cookie("session-id")})
        != m_sessions.end();
}
void Session::clearSession()
{
    if (request.hasCookie("session-id")) {
        m_sessions.erase(SessionId{request.cookie("session-id")});
    }
}
SessionData& Session::createSession(Response& response)
{
    auto sessionId = generateRandomSessionId();
    m_sessions[sessionId] = {};
    response.cookie("session-id", sessionId);
    return m_sessions[sessionId];
}
SessionData& Session::current()
{
    if (!hasValidSession()) {
        throw std::runtime_error("Invalid Session");
    }
    auto session = m_sessions.find(SessionId{request.cookie("session-id")});
    return session->second;
}
} // namespace Http