
#include "Session.hpp"

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Data/FieldTypes.hpp"

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
    m_sessions[sessionId] = {};
    response.cookie("session-id", sessionId);
    return m_sessions[sessionId];
}
SessionData& Session::current(Response& response)
{
    if (response.cookies().count("session-id")
        && m_sessions.count(SessionId{response.cookies().at("session-id")})) {
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
void Session::addAlertToSession(const Request& request, Response& response)
{
    Session session(request);
    auto& sessionData = session.current(response);
    if (sessionData.hasAlert()) {
        response.alert(
            sessionData.getAlert().message(),
            sessionData.getAlert().alertType());
        if (response.status() == Response::OK) {
            sessionData.clearAlert();
        }
    } else if (!response.alert().message().empty()) {
        sessionData.alert(response.alert());
    }
}
struct SessionDataRecord : public Record {
    SessionDataRecord(SessionId id, const SessionData& data)
        : id(id)
        , data(data)
    {
    }
    string key() const override
    {
        return id;
    };
    std::vector<string> fields() const override{
        return {"id", "user_id", "is_logged_in"};
    };
    std::map<string, string> values() const override{
        return {
            {"id", id},
            {"user_id", data.userId()},
            {"is_logged_in", data.isLoggedIn() ? "true" : "false"},
        };
    };
    HtmlInputType inputType(const string& field)  const override{
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

} // namespace Http