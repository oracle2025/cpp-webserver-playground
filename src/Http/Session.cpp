
#include "Session.hpp"

#include "Data/FieldTypes.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"

namespace Http {

map<SessionId, SessionData> Session::m_sessions;

Session::Session(const Request& request)
    : request(request)
{
}
bool Session::isLoggedIn() const
{
    return request.hasCookie("session-id")
        && (m_sessions.find(SessionId{request.cookie("session-id")})
            != m_sessions.end())
        && m_sessions[SessionId{request.cookie("session-id")}].isLoggedIn();
}
string Session::userId() const
{
    if (request.hasCookie("session-id")
        && (m_sessions.find(SessionId{request.cookie("session-id")})
            != m_sessions.end())) {

        return m_sessions[SessionId{request.cookie("session-id")}].userId();
    }
    TRACE_THROW("Session::userId() called without session-id cookie");
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
    m_sessions[sessionId] = {request.path(), request.userAgent()};
    response.cookie("session-id", sessionId);
    return m_sessions[sessionId];
}
SessionData& Session::current(Response& response)
{
    const bool hasExistingSession = response.cookies().count("session-id")
        && m_sessions.count(SessionId{response.cookies().at("session-id")});
    if (hasExistingSession) {
        return m_sessions[SessionId{response.cookies().at("session-id")}];
    }
    if (!request.hasCookie("session-id")) {
        return createSession(response);
    }
    auto session = m_sessions.find(SessionId{request.cookie("session-id")});
    if (session == m_sessions.end()) {
        return createSession(response);
    }
    return session->second;
}
bool Session::hasSession(Response& response) const
{
    if (response.cookies().count("session-id")
        && m_sessions.count(SessionId{response.cookies().at("session-id")})) {
        return true;
    }
    if (request.hasCookie("session-id")
        && m_sessions.count(SessionId{request.cookie("session-id")})) {
        return true;
    }
    return false;
}
void Session::addAlertToSession(const Request& request, Response& response)
{
    Session session(request);
    // In the next line a session is always created, even if there is no need.
    // When is no need? When the response has an alert that will be rendered in
    // the current request.
    /*
     * Cases:
     * 1) Session exists:
     *  Request has a session_id and this session_id exists
     *  Response has a session_id and this session  exists
     *
     *  if the above is not true: return
     *
     */
    if (!session.hasSession(response)) {
        return;
    }
    auto& sessionData = session.current(response);
    if (sessionData.hasAlert()) {
        // if Session has Alert then move it to the response data for rendering
        response.alert(
            sessionData.getAlert().message(),
            sessionData.getAlert().alertType());
        if (response.status() == Response::OK) {
            sessionData.clearAlert();
        }
    } else if (!response.alert().message().empty()) {
        // if the response data has an alert, it is moved to the session
        // most likely the response is a redirect
        sessionData.alert(response.alert());
    }
}
struct SessionDataRecord : public Record {
    SessionDataRecord(SessionId id, const SessionData& data)
        : id(id)
        , data(data)
    {
    }
    string presentableName() const override
    {
        return "Session";
    }
    string key() const override
    {
        return id;
    };
    std::vector<KeyStringType> fields() const override
    {
        return {
            "id",
            "user_id",
            "is_logged_in",
            "createdAt",
            "lastUsedAt",
            "path",
            "userAgent"};
    };
    std::map<KeyStringType, string> values() const override
    {
        return {
            {"id", id},
            {"user_id", data.userId()},
            {"is_logged_in", data.isLoggedInConst() ? "true" : "false"},
            {"createdAt", data.createdAt()},
            {"lastUsedAt", data.lastUsedAt()},
            {"path", data.path()},
            {"userAgent", data.userAgent()},
        };
    };
    HtmlInputType inputType(const KeyStringType& field) const override
    {
        if (field == "id") {
            return HtmlInputType::TEXT;
        }
        if (field == "user_id") {
            return HtmlInputType::TEXT;
        }
        if (field == "is_logged_in") {
            return HtmlInputType::TEXT;
        }
        return HtmlInputType::TEXT;
    };
    SessionId id;
    const SessionData& data;
};
vector<shared_ptr<Record>> Session::listAll()
{
    vector<shared_ptr<Record>> result;
    result.reserve(m_sessions.size());
    for (auto& [id, data] : m_sessions) {
        result.push_back(make_shared<SessionDataRecord>(id, data));
    }
    return result;
}
void Session::clearAll()
{
    m_sessions.clear();
}
string Session::userName() const
{
    if (request.hasCookie("session-id")
        && (m_sessions.find(SessionId{request.cookie("session-id")})
            != m_sessions.end())) {

        return m_sessions[SessionId{request.cookie("session-id")}].userName();
    }
    TRACE_THROW("Session::userId() called without session-id cookie");
}
bool Session::isAdmin() const
{
    return isLoggedIn() && userName() == "admin";
}

} // namespace Http