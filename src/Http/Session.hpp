#pragma once

#include "Login/SessionId.hpp"
#include "Http/SessionData.hpp"

#include <map>

namespace Http {

class Request;
class Response;
using std::map;

class Session {
public:
    Session(const Request& request);
    bool hasValidSession() const;
    void clearSession();
    SessionData& createSession(Response& response);
    SessionData& current();

private:
    const Request& request;
    static map<SessionId, SessionData> m_sessions;
};

} // namespace Http
