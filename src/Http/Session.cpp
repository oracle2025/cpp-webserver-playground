
#include "Session.hpp"

#include "Data/FieldTypes.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "SessionStorage.hpp"

#include <utility>

namespace Http {

Session::Session(const Request& request)
    : request(request)
{
}
bool Session::isLoggedIn() const
{
    SessionStorage storage;
    return request.hasCookie("session-id")
        && storage.pop(SessionId{request.cookie("session-id")})
        && storage.data().isLoggedIn();
}
string Session::userId() const
{
    SessionStorage storage;
    if (request.hasCookie("session-id")
        && storage.pop(SessionId{request.cookie("session-id")})) {
        return storage.data().userId();
    }
    TRACE_THROW("Session::userId() called without session-id cookie");
}
void Session::clearSession()
{
    if (request.hasCookie("session-id")) {
        SessionStorage::erase(SessionId{request.cookie("session-id")});
    }
}
SessionData Session::createSession(Response& response)
{
    auto sessionId = generateRandomSessionId();
    SessionData data{sessionId, request.path(), request.userAgent()};
    SessionStorage::insert(sessionId, data);
    response.cookie("session-id", sessionId);
    return data;
}
SessionData Session::current(Response& response)
{
    SessionStorage storage;
    const bool hasExistingSession = response.cookies().count("session-id")
        && storage.pop(SessionId{response.cookies().at("session-id")});
    if (hasExistingSession) {
        return storage.data();
    }
    if (!request.hasCookie("session-id")) {
        return createSession(response);
    }
    if (storage.pop(SessionId{request.cookie("session-id")})) {
        return storage.data();
    }
    return createSession(response);
}
bool Session::hasSession(Response& response) const
{
    if (response.cookies().count("session-id")
        && SessionStorage::count(
            SessionId{response.cookies().at("session-id")})) {
        return true;
    }
    if (request.hasCookie("session-id")
        && SessionStorage::count(SessionId{request.cookie("session-id")})) {
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
    auto sessionData = session.current(response);
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
vector<shared_ptr<Record>> Session::listAll()
{
    return SessionStorage::listAll();
}
void Session::clearAll()
{
    SessionStorage::clearAll();
}
string Session::userName() const
{
    SessionStorage storage;
    if (request.hasCookie("session-id")
        && storage.pop(SessionId{request.cookie("session-id")})) {

        return storage.data().userName();
    }
    TRACE_THROW("Session::userId() called without session-id cookie");
}
bool Session::isAdmin() const
{
    return isLoggedIn() && userName() == "admin";
}

} // namespace Http