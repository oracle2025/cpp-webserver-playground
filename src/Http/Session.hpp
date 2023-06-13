#pragma once

#include "Http/SessionData.hpp"
#include "SessionId.hpp"

#include <map>

namespace Http {

class Request;
class Response;
using std::map;

class Session {
public:
    Session(const Request& request);
    bool isLoggedIn() const;
    void clearSession();
    SessionData& createSession(Response& response);
    SessionData& current(Response& response);
    static void addAlertToSession(const Request& request, Response& response);

private:
    const Request& request;
    static map<SessionId, SessionData> m_sessions;
};

} // namespace Http
